#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "esp_adc_cal.h"

#include "K40/alerts.h"
#include "K40/flame_sensor.h"
#include "macros.h"
#include "mutex.h"
#include "queues.h"

#if HAS_DISPLAY
#include "UI/screens/status.h"
#else
#include "Headless/headless_serial.h"
#endif

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

#if HAS_DISPLAY
        // Notify UI of new value
        ui_status_notify_update(STATUS_UPDATE_PROBE_FLAME_SENSOR);
#else
        headless_notify_update(HEADLESS_UPDATE_STATUS);
#endif
    }
    RELEASE_MUTEX(flame_sensor_current_status_mutex);
}
