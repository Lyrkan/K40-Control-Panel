#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <lvgl.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "LGFX/LGFX.h"
#include "K40/bed.h"
#include "K40/cooling.h"
#include "K40/flame_sensor.h"
#include "K40/lids.h"
#include "K40/relays.h"
#include "K40/voltage_probes.h"
#include "UI/screens/status.h"
#include "UI/ui.h"
#include "api.h"
#include "settings.h"
#include "wifi.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

static AsyncWebServer server(80);
static LGFX tft;
static uint16_t touch_calibration_data[] = {274, 3922, 312, 255, 3845, 3918, 3814, 242};

static TaskHandle_t state_update_task;
static TaskHandle_t bed_update_task;

void displayFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.writePixels((lgfx::argb8888_t *)&color_p->full, w * h);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void touchpadRead(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
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
void stateUpdateTask(void *params) {
    /* Setup/calibrate ADC */
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type =
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);

    while (true) {
        // Take probes settings mutex
        while (xSemaphoreTake(probes_settings_mutex, portMAX_DELAY) != pdPASS)
            ;

        // Update sensors
        voltage_probes_update_status(&adc_chars);
        cooling_update_status(&adc_chars);
        lids_update_status();
        flame_sensor_update_status();

        // Update relays
        relays_update();

        // Release probes settings mutex
        xSemaphoreGive(probes_settings_mutex);
    }
}

/**
 * Bed update loop
 */
void bedUpdateTask(void *params) {
    bed_init();

    while (true) {
        // Take bed settings mutex
        while (xSemaphoreTake(bed_settings_mutex, portMAX_DELAY) != pdPASS)
            ;

        // Update the bed and get its new status
        BedState new_state = bed_update();

        // Release bed settings mutex
        xSemaphoreGive(bed_settings_mutex);

        // If the bed is idling don't update it for a while
        if (new_state == BED_STATE_IDLE) {
            delay(BED_IDLE_UPDATE_INTERVAL);
        }
    }
}

void setup() {
    Serial.begin(115200);

    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf[SCREEN_WIDTH * 10];
    static lv_disp_drv_t disp_drv;
    static lv_indev_drv_t indev_drv;

    /* Load settings */
    settings_init();

    /* Initialize LGFX/LVGL */
    tft.begin();
    tft.setRotation(1);
    tft.setTouchCalibrate(touch_calibration_data);
    tft.setColorDepth(24);

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * 10);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = displayFlush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpadRead;
    lv_indev_drv_register(&indev_drv);

    /* Setup pins */
    pinMode(PIN_VOLTAGE_PROBE_1, INPUT);
    pinMode(PIN_VOLTAGE_PROBE_2, INPUT);
    pinMode(PIN_VOLTAGE_PROBE_3, INPUT);
    pinMode(PIN_LID_STATUS_FRONT, INPUT);
    pinMode(PIN_LID_STATUS_BACK, INPUT_PULLUP);
    pinMode(PIN_FLAME_SENSOR, INPUT_PULLUP);
    pinMode(PIN_RELAY_LASER, OUTPUT);
    pinMode(PIN_RELAY_AIR_ASSIST, OUTPUT);
    pinMode(PIN_RELAY_COOLING, OUTPUT);
    pinMode(PIN_RELAY_ALARM, OUTPUT);
    pinMode(PIN_RELAY_LIGHTS, OUTPUT);
    pinMode(PIN_RELAY_BEAM_PREVIEW, OUTPUT);
    pinMode(PIN_COOLING_THERMISTOR, INPUT);
    pinMode(PIN_COOLING_FLOW, INPUT_PULLUP);
    pinMode(PIN_BED_STEP, OUTPUT);
    pinMode(PIN_BED_DIR, OUTPUT);
    pinMode(PIN_BED_LIMIT, INPUT_PULLUP);

    /* Setup interrupts */
    attachInterrupt(PIN_COOLING_FLOW, cooling_flow_probe_interrupt, FALLING);

    /* Initialize relays state/pins */
    relays_init();

    /* Initialize UI */
    ui_init();

    /* Initialize WiFi */
    wifi_init();

    /* Initialize API */
    api_init(&server);

    /* Initialize ElegantOTA */
    // TODO Add authentication
    AsyncElegantOTA.begin(&server);
    server.begin();

    /* Start state update loop */
    xTaskCreatePinnedToCore(
        stateUpdateTask,
        "StateUpdateTask",
        10000,
        NULL,
        0,
        &state_update_task,
        0 // Run on core #0, UI will be updated by loop() in core#1
    );

    /* Start bed update loop */
    xTaskCreatePinnedToCore(
        bedUpdateTask,
        "BedUpdateTask",
        10000,
        NULL,
        0,
        &bed_update_task,
        0 // Run on core #0, UI will be updated by loop() in core#1
    );
}

/**
 * UI update loop
 */
void loop() {
    ui_update();
    lv_timer_handler();
    delay(5);
}
