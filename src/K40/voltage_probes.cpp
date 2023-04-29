#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <math.h>
#include "esp_adc_cal.h"

#include "K40/alerts.h"
#include "K40/voltage_probes.h"
#include "UI/screens/status.h"
#include "queues.h"
#include "settings.h"

static VoltageProbesValues voltage_probes_values;

static float_t voltage_probes_get_value(VoltageProbePin pin, esp_adc_cal_characteristics_t *adc_chars) {
    float_t voltage = (float_t)esp_adc_cal_raw_to_voltage(analogRead(pin), adc_chars);

    // If the value is under 150mV it probably means we're
    // not able to get an accurate reading from the ESP32
    // ADC pin.
    if (voltage < 150) {
        voltage = 0;
    }

    // Bring the value back to 0-50V range
    return (16.36 * voltage) / 1000.0;
}

void voltage_probes_update_status(esp_adc_cal_characteristics_t *adc_chars) {
    static unsigned long last_update = 0;
    unsigned long current_time = millis();
    if (last_update == 0) {
        last_update = current_time;
    }

    unsigned long delta_time = current_time - last_update;
    if (delta_time < VOLTAGE_PROBES_UPDATE_INTERVAL) {
        return;
    }

    /* Retrieve voltage probes values */
    voltage_probes_values.probe1 = voltage_probes_get_value(PIN_VOLTAGE_PROBE_1, adc_chars);
    voltage_probes_values.probe2 = voltage_probes_get_value(PIN_VOLTAGE_PROBE_2, adc_chars);
    voltage_probes_values.probe3 = voltage_probes_get_value(PIN_VOLTAGE_PROBE_3, adc_chars);
    xQueueOverwrite(voltage_current_status_queue, &voltage_probes_values);

    // Change alert state
    bool enable_alert =
        (voltage_probes_values.probe1 < probes_settings.voltage_probe_v1_min ||
         voltage_probes_values.probe1 > probes_settings.voltage_probe_v1_max ||
         voltage_probes_values.probe1 < probes_settings.voltage_probe_v2_min ||
         voltage_probes_values.probe1 > probes_settings.voltage_probe_v2_max ||
         voltage_probes_values.probe2 < probes_settings.voltage_probe_v3_min ||
         voltage_probes_values.probe2 > probes_settings.voltage_probe_v3_max);

    alerts_toggle_alert(ALERT_TYPE_VOLTAGE, enable_alert);

    // Notify UI of new values
    ui_status_notify_update(STATUS_UPDATE_PROBE_VOLTAGE);

    // Reset timer
    last_update = current_time;
}
