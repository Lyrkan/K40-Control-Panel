#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <lvgl.h>
#include <math.h>
#include <SPIFFS.h>

#include "K40/alerts.h"
#include "K40/cooling.h"
#include "K40/lids.h"
#include "K40/voltage_probes.h"
#include "K40/relays.h"
#include "UI/display.h"
#include "cpu_monitor.h"
#include "macros.h"
#include "queues.h"
#include "settings.h"
#include "webserver.h"

SemaphoreHandle_t webserver_mutex = xSemaphoreCreateRecursiveMutex();

static AsyncWebServer server(80);

static String getContentType(String path) {
    if (path.endsWith(".html") || path.endsWith(".html")) {
        return String("text/html");
    }

    if (path.endsWith(".js")) {
        return String("text/javascript");
    }

    if (path.endsWith(".css")) {
        return String("text/css");
    }

    if (path.endsWith(".png")) {
        return String("image/png");
    }

    if (path.endsWith(".jpg") || path.endsWith("jpeg")) {
        return String("image/jpeg");
    }

    return String("application/octet-stream");
}

static void handleStatusRequest(AsyncWebServerRequest *request) {
    // Prevent UI updates to avoid heap issues
    TAKE_RECURSIVE_MUTEX(webserver_mutex)

    DynamicJsonDocument state(1024);
    String serializedState;

    state["firmware"]["version"] = GIT_CURRENT_REF;
    state["firmware"]["build_date"] = __DATE__ " " __TIME__;

    // Retrieve sensors values
    VoltageProbesValues voltage_probes_values;
    CoolingValues cooling_values;
    LidsStates lids_states;
    bool flame_sensor_triggered;

    if (xQueuePeek(voltage_current_status_queue, &voltage_probes_values, 100.f / portTICK_RATE_MS) == pdTRUE) {
        state["sensors"]["voltages"]["v1"] = voltage_probes_values.probe1;
        state["sensors"]["voltages"]["v2"] = voltage_probes_values.probe2;
        state["sensors"]["voltages"]["v3"] = voltage_probes_values.probe3;
    } else {
        state["sensors"]["voltages"]["v1"] = nullptr;
        state["sensors"]["voltages"]["v2"] = nullptr;
        state["sensors"]["voltages"]["v3"] = nullptr;
    }

    if (xQueuePeek(cooling_current_status_queue, &cooling_values, 100.f / portTICK_RATE_MS) == pdTRUE) {
        state["sensors"]["cooling"]["flow"] = cooling_values.flow;
        state["sensors"]["cooling"]["temp"] = cooling_values.temperature;
    } else {
        state["sensors"]["cooling"]["flow"] = nullptr;
        state["sensors"]["cooling"]["temp"] = nullptr;
    }

    if (xQueuePeek(lids_current_status_queue, &lids_states, 100.f / portTICK_RATE_MS) == pdTRUE) {
        state["sensors"]["lids"]["front"] = lids_states.front_opened ? "opened" : "closed";
        state["sensors"]["lids"]["back"] = lids_states.back_opened ? "opened" : "closed";
    } else {
        state["sensors"]["lids"]["front"] = nullptr;
        state["sensors"]["lids"]["back"] = nullptr;
    }

    if (xQueuePeek(flame_sensor_current_status_queue, &flame_sensor_triggered, 0) == pdTRUE) {
        state["sensors"]["flame_sensor"]["triggered"] = flame_sensor_triggered;
    } else {
        state["sensors"]["flame_sensor"]["triggered"] = nullptr;
    }

    // Retrieve alerts
    uint8_t alerts_status = alerts_get_current_alerts();
    state["alerts"]["voltages"] = (alerts_status & ALERT_TYPE_VOLTAGE) != 0;
    state["alerts"]["cooling"] = (alerts_status & ALERT_TYPE_COOLING) != 0;
    state["alerts"]["lids"] = (alerts_status & ALERT_TYPE_LIDS) != 0;
    state["alerts"]["flame_sensor"] = (alerts_status & ALERT_TYPE_FLAME_SENSOR) != 0;

    // Retrieve relays state from the queue object
    RelaysStatus current_relays_status;
    if (xQueuePeek(relays_current_status_queue, &current_relays_status, 0) == pdTRUE) {
        state["relays"]["laser"] = current_relays_status.laser_enabled;
        state["relays"]["air_assist"] = current_relays_status.air_assist_enabled;
        state["relays"]["cooling"] = current_relays_status.cooling_enabled;
        state["relays"]["alarm"] = current_relays_status.alarm_enabled;
        state["relays"]["lights"] = current_relays_status.lights_enabled;
        state["relays"]["beam_preview"] = current_relays_status.beam_preview_enabled;
    } else {
        state["relays"]["laser"] = nullptr;
        state["relays"]["air_assist"] = nullptr;
        state["relays"]["cooling"] = nullptr;
        state["relays"]["alarm"] = nullptr;
        state["relays"]["lights"] = nullptr;
        state["relays"]["beam_preview"] = nullptr;
    }

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

    // Serialize JSON data and send it to the client
    serializeJson(state, serializedState);
    request->sendChunked(
        "application/json",
        [serializedState](uint8_t *buffer, size_t max_len, size_t index) -> size_t {
            size_t bytes = min(serializedState.length() - index, max_len);
            for (int i = 0; i < bytes; i++) {
                buffer[i] = serializedState.charAt(index + i);
            }

            if (bytes <= 0) {
                RELEASE_RECURSIVE_MUTEX(webserver_mutex)
            }

            return max((size_t)0, bytes);
        });
}

static bool handleStaticFileRequest(AsyncWebServerRequest *request) {
    String path = request->url();
    if (path.endsWith("/")) {
        path += String("index.html");
    }

    String pathWithGz = path + ".gz";

    if (!SPIFFS.exists(pathWithGz) && !SPIFFS.exists(path)) {
        path = String("/index.html");
    }

    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
        // Prevent UI updates to avoid heap issues
        TAKE_RECURSIVE_MUTEX(webserver_mutex)

        bool gzipped = false;
        if (SPIFFS.exists(pathWithGz)) {
            gzipped = true;
        }

        File file = SPIFFS.open(gzipped ? pathWithGz : path, "r");
        AsyncWebServerResponse *response = request->beginResponse(
            getContentType(path),
            file.size(),
            [file](uint8_t *buffer, size_t maxLen, size_t total) mutable -> size_t {
                size_t bytes = file.read(buffer, maxLen);
                if (bytes <= 0 || bytes + total == file.size()) {
                    RELEASE_RECURSIVE_MUTEX(webserver_mutex)
                    file.close();
                }

                return max((size_t)0, bytes);
            });

        if (gzipped) {
            response->addHeader(String("Content-Encoding"), String("gzip"));
        }

        request->send(response);
        return true;
    }

    return false;
}

static void handleScreenshotRequest(AsyncWebServerRequest *request) {
    // Prevent UI updates
    TAKE_RECURSIVE_MUTEX(webserver_mutex)

    lv_obj_t *current_screen = lv_scr_act();

    request->sendChunked(
        "application/octet-stream",
        [current_screen](uint8_t *buffer, size_t max_len, size_t index) -> size_t {
            uint8_t bytes_per_pixel = 4;
            unsigned int current_pixel_index = index / bytes_per_pixel;
            if (current_pixel_index >= DISPLAY_SCREEN_HEIGHT * DISPLAY_SCREEN_WIDTH) {
                RELEASE_RECURSIVE_MUTEX(webserver_mutex)
                return 0;
            }

            unsigned int row = current_pixel_index / DISPLAY_SCREEN_WIDTH;
            unsigned int column = (current_pixel_index % DISPLAY_SCREEN_WIDTH);
            unsigned int remaining_columns = DISPLAY_SCREEN_WIDTH - column;
            unsigned int max_pixels = min(max_len / bytes_per_pixel, remaining_columns);

            if (max_pixels == 0) {
                RELEASE_RECURSIVE_MUTEX(webserver_mutex)
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
            lv_disp_drv_use_generic_set_px_cb(&driver, LV_IMG_CF_TRUE_COLOR_ALPHA);

            lv_disp_t fake_disp;
            lv_memset_00(&fake_disp, sizeof(lv_disp_t));
            fake_disp.driver = &driver;

            lv_draw_ctx_t *draw_ctx = (lv_draw_ctx_t *)lv_mem_alloc(obj_disp->driver->draw_ctx_size);
            if (draw_ctx == NULL) {
                RELEASE_RECURSIVE_MUTEX(webserver_mutex);
                return 0;
            }

            obj_disp->driver->draw_ctx_init(fake_disp.driver, draw_ctx);
            fake_disp.driver->draw_ctx = draw_ctx;
            draw_ctx->clip_area = &snapshot_area;
            draw_ctx->buf_area = &snapshot_area;
            draw_ctx->buf = (void *)buffer;
            driver.draw_ctx = draw_ctx;

            lv_disp_t *refr_ori = _lv_refr_get_disp_refreshing();
            _lv_refr_set_disp_refreshing(&fake_disp);

            lv_obj_redraw(draw_ctx, current_screen);
            lv_obj_redraw(draw_ctx, lv_layer_top());
            lv_obj_redraw(draw_ctx, lv_layer_sys());

            _lv_refr_set_disp_refreshing(refr_ori);
            obj_disp->driver->draw_ctx_deinit(fake_disp.driver, draw_ctx);
            lv_mem_free(draw_ctx);

            return max_pixels * bytes_per_pixel;
        });
}

void webserver_init() {
    // APIs
    server.on("/api/status", HTTP_GET, handleStatusRequest);

    // Screenshot utility
    // Usage:
    // $ wget http://<YOUR_PANEL_IP>/screenshot
    // $ convert -size 480x320 -depth 8 -separate -swap 0,2 -combine rgba:screenshot screenshot.jpg
    server.on("/screenshot", HTTP_GET, handleScreenshotRequest);

    // Static files
    if (SPIFFS.begin()) {
        server.onNotFound([](AsyncWebServerRequest *request) {
            if (handleStaticFileRequest(request)) {
                return;
            }

            request->send(404);
        });
    }

    /* Initialize ElegantOTA */
    TAKE_MUTEX(ota_settings_mutex)
    AsyncElegantOTA.begin(&server, ota_settings.login, ota_settings.password);
    RELEASE_MUTEX(ota_settings_mutex)

    // Start server
    server.begin();
}
