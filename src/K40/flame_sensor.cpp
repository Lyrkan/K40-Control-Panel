#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "esp_adc_cal.h"

#include "K40/alerts.h"
#include "K40/flame_sensor.h"
#include "UI/screens/status.h"
#include "macros.h"
#include "mutex.h"
#include "queues.h"

bool flame_sensor_triggered;

void flame_sensor_update_status() {
    TAKE_MUTEX(flame_sensor_current_status_mutex);
    static bool first_update = true;
    bool updated_sensor_value = digitalRead(PIN_FLAME_SENSOR) != HIGH;

    if (first_update || (updated_sensor_value != flame_sensor_triggered)) {
        first_update = false;
        flame_sensor_triggered = updated_sensor_value;

        // Change alert state
        alerts_toggle_alert(ALERT_TYPE_FLAME_SENSOR, flame_sensor_triggered);

        // Notify UI of new value
        ui_status_notify_update(STATUS_UPDATE_PROBE_FLAME_SENSOR);
    }
    RELEASE_MUTEX(flame_sensor_current_status_mutex);
}
