#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <AsyncJson.h>
#include <ElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include <math.h>
#include <SPIFFS.h>

#include "Grbl/grbl_report.h"
#include "Grbl/grbl_state.h"
#include "K40/alerts.h"
#include "K40/cooling.h"
#include "K40/flame_sensor.h"
#include "K40/lids.h"
#include "K40/relays.h"
#include "cpu_monitor.h"
#include "esp.h"
#include "macros.h"
#include "mutex.h"
#include "queues.h"
#include "settings.h"
#include "tasks.h"
#include "webserver.h"

#if HAS_DISPLAY
#include <lvgl.h>
#include "UI/display.h"
#endif

SemaphoreHandle_t webserver_screenshot_mutex = xSemaphoreCreateRecursiveMutex();
static AsyncWebServer server(80);

static void handleApiInfoRequest(AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &state = response->getRoot();

    state["firmware"]["version"] = GIT_CURRENT_REF;
    state["firmware"]["build_date"] = __DATE__ " " __TIME__;

    // Retrieve system data
    TAKE_MUTEX(cpu_monitor_stats_mutex)
    float_t core_usage_percentage_0 = cpu_monitor_load_0;
    float_t core_usage_percentage_1 = cpu_monitor_load_1;
    RELEASE_MUTEX(cpu_monitor_stats_mutex)

    state["system"]["chip"]["model"] = ESP.getChipModel();
    state["system"]["chip"]["revision"] = ESP.getChipRevision();
    state["system"]["heap"]["free"] = ESP.getFreeHeap();
    state["system"]["heap"]["total"] = ESP.getHeapSize();
    state["system"]["cpu"]["freq_mhz"] = ESP.getCpuFreqMHz();
    state["system"]["cpu"]["load_percent"]["core_0"] = core_usage_percentage_0;
    state["system"]["cpu"]["load_percent"]["core_1"] = core_usage_percentage_1;

    TaskHandle_t task_handles[] = {
#if HAS_DISPLAY
        display_update_task_handle,
#else
        headless_rx_task_handle,
        headless_tx_task_handle,
        headless_status_update_task_handle,
#endif
        bed_update_task_handle,
        state_update_task_handle,
        grbl_rx_task_handle,
        grbl_tx_task_handle,
        settings_save_task_handle,
        cpu_monitor_task_handle,
        xTaskGetCurrentTaskHandle()
    };

    const char *task_states[] = {
        "running",
        "ready",
        "blocked",
        "suspended",
        "deleted",
    };

    for (int task_index = 0; task_index < ARRAY_SIZE(task_handles); task_index++) {
        char *task_name = pcTaskGetName(task_handles[task_index]);
        eTaskState task_state = eTaskGetState(task_handles[task_index]);
        state["system"]["tasks"][task_name]["state"] =
            (task_state >= 0 && task_state < ARRAY_SIZE(task_states)) ? task_states[task_state] : "invalid";
        state["system"]["tasks"][task_name]["priority"] = uxTaskPriorityGet(task_handles[task_index]);
        state["system"]["tasks"][task_name]["high_water_mark"] = uxTaskGetStackHighWaterMark(task_handles[task_index]);
    }

    state["system"]["reset_reason"] = esp_reset_reason_as_text();

    // Serialize JSON data and send it to the client
    response->setLength();
    request->send(response);
}

static void handleApiSensorsRequest(AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &state = response->getRoot();

    TAKE_MUTEX(cooling_current_status_mutex)
    state["sensors"]["cooling"]["flow"]["input"] = cooling_values.input_flow;
    state["sensors"]["cooling"]["flow"]["output"] = cooling_values.output_flow;
    state["sensors"]["cooling"]["temp"]["input"] = cooling_values.input_temperature;
    state["sensors"]["cooling"]["temp"]["output"] = cooling_values.output_temperature;
    RELEASE_MUTEX(cooling_current_status_mutex)

    TAKE_MUTEX(lids_current_status_mutex)
    state["sensors"]["lids"]["front"] = lids_states.front_opened ? "opened" : "closed";
    state["sensors"]["lids"]["back"] = lids_states.back_opened ? "opened" : "closed";
    RELEASE_MUTEX(lids_current_status_mutex)

    TAKE_MUTEX(flame_sensor_current_status_mutex)
    state["sensors"]["flame_sensor"]["triggered"] = flame_sensor_triggered;
    RELEASE_MUTEX(flame_sensor_current_status_mutex)

    // Serialize JSON data and send it to the client
    response->setLength();
    request->send(response);
}

static void handleApiAlertsRequest(AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &state = response->getRoot();

    // Retrieve alerts
    uint8_t alerts_status = alerts_get_current_alerts();
    state["alerts"]["cooling"] = (alerts_status & ALERT_TYPE_COOLING) != 0;
    state["alerts"]["lids"] = (alerts_status & ALERT_TYPE_LIDS) != 0;
    state["alerts"]["flame_sensor"] = (alerts_status & ALERT_TYPE_FLAME_SENSOR) != 0;

    // Serialize JSON data and send it to the client
    response->setLength();
    request->send(response);
}

static void handleApiRelaysRequest(AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &state = response->getRoot();

    // Retrieve relays state
    state["relays"]["interlock"] = relays_is_active(RELAY_PIN_INTERLOCK);
    state["relays"]["alarm"] = relays_is_active(RELAY_PIN_ALARM);
    state["relays"]["lights"] = relays_is_active(RELAY_PIN_LIGHTS);
    state["relays"]["beam_preview"] = relays_is_active(RELAY_PIN_BEAM_PREVIEW);

    // Serialize JSON data and send it to the client
    response->setLength();
    request->send(response);
}

static void handleGrblStatusRequest(AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonVariant &state = response->getRoot();

    TAKE_MUTEX(grbl_last_report_mutex)

    state["state"] = grbl_last_report.state;

    if (grbl_last_report.w_pos.is_set) {
        state["w_pos"]["x"] = grbl_last_report.w_pos.x;
        state["w_pos"]["y"] = grbl_last_report.w_pos.y;
        state["w_pos"]["z"] = grbl_last_report.w_pos.z;
    } else {
        state["w_pos"] = nullptr;
    }

    if (grbl_last_report.m_pos.is_set) {
        state["m_pos"]["x"] = grbl_last_report.m_pos.x;
        state["m_pos"]["y"] = grbl_last_report.m_pos.y;
        state["m_pos"]["z"] = grbl_last_report.m_pos.z;
    } else {
        state["w_pos"] = nullptr;
    }

    if (grbl_last_report.wco.is_set) {
        state["wco"]["x"] = grbl_last_report.wco.x;
        state["wco"]["y"] = grbl_last_report.wco.y;
        state["wco"]["z"] = grbl_last_report.wco.z;
    } else {
        state["wco"] = nullptr;
    }

    if (grbl_last_report.buffer_state.is_set) {
        state["buffer"]["planned_buffer_available_blocks"] =
            grbl_last_report.buffer_state.planned_buffer_available_blocks;
        state["buffer"]["rx_buffer_available_bytes"] = grbl_last_report.buffer_state.rx_buffer_available_bytes;
    } else {
        state["buffer"] = nullptr;
    }

    if (grbl_last_report.feed_state.is_set) {
        state["feed"]["rate"] = grbl_last_report.feed_state.rate;
        state["feed"]["spindle_speed"] = grbl_last_report.feed_state.spindle_speed;
    } else {
        state["feed"] = nullptr;
    }

    state["line_number"] = grbl_last_report.line_number;

    int active_pins = grbl_last_report.active_pins;
    RELEASE_MUTEX(grbl_last_report_mutex)

    state["active_pins"]["x"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_X) != 0;
    state["active_pins"]["y"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_Y) != 0;
    state["active_pins"]["z"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_Z) != 0;
    state["active_pins"]["p"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_P) != 0;
    state["active_pins"]["d"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_D) != 0;
    state["active_pins"]["h"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_H) != 0;
    state["active_pins"]["r"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_R) != 0;
    state["active_pins"]["s"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_S) != 0;

    // Serialize JSON data and send it to the client
    response->setLength();
    request->send(response);
}

static void handleScreenshotRequest(AsyncWebServerRequest *request) {
#if HAS_DISPLAY
    // Prevent UI updates
    TAKE_RECURSIVE_MUTEX(webserver_screenshot_mutex)

    lv_obj_t *current_screen = lv_scr_act();

    request->sendChunked("image/bmp", [current_screen](uint8_t *buffer, size_t max_len, size_t index) -> size_t {
        typedef struct __attribute__((__packed__)) {
            char bmp_signature[2];
            int32_t bmp_size;
            int32_t bmp_reserved;
            int32_t bmp_offset;
            int32_t dib_header_size;
            int32_t dib_width;
            int32_t dib_height;
            int16_t dib_color_planes;
            int16_t dib_bpp;
            int32_t dib_compression;
            int32_t dib_bmp_size;
            int32_t dib_hor_res;
            int32_t dib_ver_res;
            int32_t dib_palette;
            int32_t dib_important_colors;
            int32_t bitfield_mask_r;
            int32_t bitfield_mask_g;
            int32_t bitfield_mask_b;
        } BitmapHeader;

        // Add bitmap header if this is the first chunk
        size_t start_index = 0;
        size_t header_size = sizeof(BitmapHeader);
        uint8_t bytes_per_pixel = 2;

        if (index == 0) {
            // TODO Make it so the header can still be sent
            // even if the first buffer is less than header_size bytes.
            if (max_len < header_size) {
                return 0;
            }

            BitmapHeader *bmp_header = (BitmapHeader *)buffer;
            sprintf(bmp_header->bmp_signature, "BM");
            bmp_header->bmp_size = header_size + (bytes_per_pixel * DISPLAY_SCREEN_WIDTH * DISPLAY_SCREEN_HEIGHT);
            bmp_header->bmp_offset = header_size;
            bmp_header->dib_header_size = header_size - 14;
            bmp_header->dib_width = DISPLAY_SCREEN_WIDTH;
            bmp_header->dib_height = -DISPLAY_SCREEN_HEIGHT;
            bmp_header->dib_color_planes = 1;
            bmp_header->dib_bpp = bytes_per_pixel * 8;
            bmp_header->dib_compression = 3; // BI_BITFIELDS
            bmp_header->dib_bmp_size = (bytes_per_pixel * DISPLAY_SCREEN_WIDTH * DISPLAY_SCREEN_HEIGHT);
            bmp_header->dib_hor_res = 5000;
            bmp_header->dib_ver_res = 5000;
            bmp_header->dib_palette = 0;
            bmp_header->dib_important_colors = 0;
            bmp_header->bitfield_mask_r = 0xF800;
            bmp_header->bitfield_mask_g = 0x07E0;
            bmp_header->bitfield_mask_b = 0x001F;

            start_index = header_size;
            max_len -= header_size;
        }

        unsigned int current_pixel_index = (index - (header_size - start_index)) / bytes_per_pixel;
        if (current_pixel_index >= DISPLAY_SCREEN_HEIGHT * DISPLAY_SCREEN_WIDTH) {
            RELEASE_RECURSIVE_MUTEX(webserver_screenshot_mutex)
            return 0;
        }

        unsigned int row = current_pixel_index / DISPLAY_SCREEN_WIDTH;
        unsigned int column = (current_pixel_index % DISPLAY_SCREEN_WIDTH);
        unsigned int remaining_columns = DISPLAY_SCREEN_WIDTH - column;
        unsigned int max_pixels = min(max_len / bytes_per_pixel, remaining_columns);

        if (max_pixels == 0) {
            RELEASE_RECURSIVE_MUTEX(webserver_screenshot_mutex)
            return 0;
        }

        lv_area_t snapshot_area = {
            .x1 = (lv_coord_t)column,
            .y1 = (lv_coord_t)row,
            .x2 = (lv_coord_t)(column + max_pixels - 1),
            .y2 = (lv_coord_t)row,
        };

        lv_disp_t *obj_disp = lv_obj_get_disp(current_screen);
        lv_disp_drv_t driver;
        lv_disp_drv_init(&driver);

        driver.hor_res = lv_disp_get_hor_res(obj_disp);
        driver.ver_res = lv_disp_get_hor_res(obj_disp);
        lv_disp_drv_use_generic_set_px_cb(&driver, LV_IMG_CF_RGB565);

        lv_disp_t fake_disp;
        lv_memset_00(&fake_disp, sizeof(lv_disp_t));
        fake_disp.driver = &driver;

        lv_draw_ctx_t *draw_ctx = (lv_draw_ctx_t *)lv_mem_alloc(obj_disp->driver->draw_ctx_size);
        if (draw_ctx == NULL) {
            RELEASE_RECURSIVE_MUTEX(webserver_screenshot_mutex);
            return 0;
        }

        obj_disp->driver->draw_ctx_init(fake_disp.driver, draw_ctx);
        fake_disp.driver->draw_ctx = draw_ctx;
        draw_ctx->clip_area = &snapshot_area;
        draw_ctx->buf_area = &snapshot_area;
        draw_ctx->buf = (void *)(buffer + start_index);
        driver.draw_ctx = draw_ctx;

        lv_disp_t *refr_ori = _lv_refr_get_disp_refreshing();
        _lv_refr_set_disp_refreshing(&fake_disp);

        lv_obj_redraw(draw_ctx, current_screen);
        lv_obj_redraw(draw_ctx, lv_layer_top());
        lv_obj_redraw(draw_ctx, lv_layer_sys());

        _lv_refr_set_disp_refreshing(refr_ori);
        obj_disp->driver->draw_ctx_deinit(fake_disp.driver, draw_ctx);
        lv_mem_free(draw_ctx);

        return start_index + (max_pixels * bytes_per_pixel);
    });
#else
    request->send(404, "text/plain", "K40-Control-Panel was compiled without display support");
#endif
}

static void handleNotFoundRequest(AsyncWebServerRequest *request) { request->send(404, "text/plain", "Not Found"); }

void webserver_init() {
    // APIs
    server.on("/api/info", HTTP_GET, handleApiInfoRequest);
    server.on("/api/sensors", HTTP_GET, handleApiSensorsRequest);
    server.on("/api/alerts", HTTP_GET, handleApiAlertsRequest);
    server.on("/api/relays", HTTP_GET, handleApiRelaysRequest);
    server.on("/api/grbl", HTTP_GET, handleGrblStatusRequest);
    server.on("/screenshot", HTTP_GET, handleScreenshotRequest);

    // 404
    server.onNotFound(handleNotFoundRequest);

    /* Initialize ElegantOTA */
    TAKE_MUTEX(ota_settings_mutex)
    ElegantOTA.begin(&server, ota_settings.login, ota_settings.password);
    RELEASE_MUTEX(ota_settings_mutex)

    // Start server
    server.begin();
}
