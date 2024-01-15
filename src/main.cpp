#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <lvgl.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "LGFX/LGFX.h"
#include "K40/bed.h"
#include "K40/cooling.h"
#include "K40/flame_sensor.h"
#include "K40/lids.h"
#include "K40/relays.h"
#include "UI/screens/status.h"
#include "UI/display.h"
#include "UI/ui.h"
#include "cpu_monitor.h"
#include "macros.h"
#include "settings.h"
#include "webserver.h"
#include "wifi.h"

static LGFX tft;
static uint16_t touch_calibration_data[] = {274, 3922, 312, 255, 3845, 3918, 3814, 242};

static TaskHandle_t state_update_task_handle;
static TaskHandle_t bed_update_task_handle;

void display_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixels((lgfx::argb8888_t *)&color_p->full, w * h);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

static void touchpad_read_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t touchX, touchY;
    bool touched = tft.getTouch(&touchX, &touchY);
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

/**
 * State update loop
 */
void state_update_task_func(void *params) {
    /* Setup/calibrate ADC */
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type =
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);

    while (true) {
        // Take probes settings mutex
        TAKE_MUTEX(probes_settings_mutex)

        // Update sensors
        cooling_update_status(&adc_chars);
        lids_update_status();
        flame_sensor_update_status();

        // Update relays
        relays_update();

        // Release probes settings mutex
        RELEASE_MUTEX(probes_settings_mutex)

        // Let other tasks do their thing
        yield();
    }
}

/**
 * Bed update loop
 */
void bed_update_task_func(void *params) {
    bed_init();

    while (true) {
        // Take bed settings mutex
        TAKE_MUTEX(bed_settings_mutex)

        // Update the bed and get its new status
        BedState new_state = bed_update();

        // Release bed settings mutex
        RELEASE_MUTEX(bed_settings_mutex)

        if (new_state == BED_STATE_IDLE) {
            // If the bed is idling don't update it for a while
            vTaskDelay(pdMS_TO_TICKS(BED_IDLE_UPDATE_INTERVAL));
        } else {
            // Let other tasks do their thing
            yield();
        }
    }
}

void setup() {
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf[DISPLAY_SCREEN_WIDTH * 10];
    static lv_disp_drv_t disp_drv;
    static lv_indev_drv_t indev_drv;

    Serial.begin(115200);
    log_i("K40 Control Panel (%s)", GIT_CURRENT_REF);

    /* Load settings */
    settings_init();

    /* Initialize LGFX/LVGL */
    tft.begin();
    tft.setRotation(1);
    tft.setTouchCalibrate(touch_calibration_data);
    tft.setColorDepth(24);

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, DISPLAY_SCREEN_WIDTH * 10);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = DISPLAY_SCREEN_WIDTH;
    disp_drv.ver_res = DISPLAY_SCREEN_HEIGHT;
    disp_drv.flush_cb = display_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read_cb;
    lv_indev_drv_register(&indev_drv);

    /* Setup pins */
    pinMode(PIN_LID_STATUS_FRONT, INPUT);
    pinMode(PIN_LID_STATUS_BACK, INPUT_PULLUP);
    pinMode(PIN_FLAME_SENSOR, INPUT_PULLUP);
    pinMode(RELAY_PIN_INTERLOCK, OUTPUT);
    pinMode(RELAY_PIN_AIR_ASSIST, OUTPUT);
    pinMode(RELAY_PIN_ALARM, OUTPUT);
    pinMode(RELAY_PIN_LIGHTS, OUTPUT);
    pinMode(RELAY_PIN_BEAM_PREVIEW, OUTPUT);
    pinMode(PIN_COOLING_THERMISTOR_IN, INPUT);
    pinMode(PIN_COOLING_FLOW_IN, INPUT_PULLUP);
    pinMode(PIN_COOLING_THERMISTOR_OUT, INPUT);
    pinMode(PIN_COOLING_FLOW_OUT, INPUT_PULLUP);
    pinMode(PIN_BED_STEP, OUTPUT);
    pinMode(PIN_BED_DIR, OUTPUT);
    pinMode(PIN_BED_LIMIT, INPUT_PULLUP);

    /* Set default state for relays */
    relays_init();

    /* Setup interrupts */
    attachInterrupt(PIN_COOLING_FLOW_IN, cooling_flow_input_probe_interrupt, FALLING);
    attachInterrupt(PIN_COOLING_FLOW_OUT, cooling_flow_output_probe_interrupt, FALLING);

    /* Initialize UI */
    ui_init();

    /* Initialize WiFi */
    wifi_init();

    /* Initialize CPU monitoring task */
    cpu_monitor_init();

    /* Initialize Webserver */
    webserver_init();

    /* Start state update loop */
    xTaskCreatePinnedToCore(
        state_update_task_func,
        "state_update_task",
        10000,
        NULL,
        0,
        &state_update_task_handle,
        0 // Run on core #0, UI will be updated by loop() in core#1
    );

    /* Start bed update loop */
    xTaskCreatePinnedToCore(
        bed_update_task_func,
        "bed_update_task",
        10000,
        NULL,
        0,
        &bed_update_task_handle,
        0 // Run on core #0, UI will be updated by loop() in core#1
    );
}

/**
 * UI update loop
 */
void loop() {
    // Avoid updating the screen when the webserver is taking a screenshot
    TAKE_RECURSIVE_MUTEX(webserver_screenshot_mutex)
    ui_update();
    lv_timer_handler();
    RELEASE_RECURSIVE_MUTEX(webserver_screenshot_mutex)

    vTaskDelay(pdMS_TO_TICKS(5));
}
