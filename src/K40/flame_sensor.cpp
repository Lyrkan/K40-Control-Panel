#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "esp_adc_cal.h"

#include "K40/flame_sensor.h"
#include "UI/screens/status.h"
#include "queues.h"

static bool flame_sensor_triggered;

void flame_sensor_update_status() {
    static bool first_update = true;
    bool updated_sensor_value = digitalRead(PIN_FLAME_SENSOR) != HIGH;

    if (first_update || (updated_sensor_value != flame_sensor_triggered)) {
        first_update = false;
        flame_sensor_triggered = updated_sensor_value;
        xQueueOverwrite(flame_sensor_status_update_queue, &flame_sensor_triggered);
    }
}
