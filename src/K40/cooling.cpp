#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <math.h>
#include "esp_adc_cal.h"

#include "K40/alerts.h"
#include "K40/cooling.h"
#include "macros.h"
#include "mutex.h"
#include "settings.h"

#if HAS_DISPLAY
#include "UI/screens/status.h"
#else
#include "Headless/headless_serial.h"
#endif

CoolingValues cooling_values;

static volatile uint32_t cooling_flow_input_interrupt_counter = 0;
void IRAM_ATTR cooling_flow_input_probe_interrupt() { cooling_flow_input_interrupt_counter++; }

static volatile uint32_t cooling_flow_output_interrupt_counter = 0;
void IRAM_ATTR cooling_flow_output_probe_interrupt() { cooling_flow_output_interrupt_counter++; }

static float_t cooling_seinhart_hart_temperature(float_t thermistor_value) {
    float_t temperature = log(thermistor_value / COOLING_THERMISTOR_NOMINAL_RESISTANCE);
    temperature /= COOLING_THERMISTOR_COEFF;
    temperature += 1.0 / (COOLING_THERMISTOR_NOMINAL_TEMPERATURE + 273.15);
    temperature = 1.0 / temperature;
    temperature -= 273.15;
    return temperature;
}

void cooling_update_status(esp_adc_cal_characteristics_t *adc_chars) {
    TAKE_MUTEX(cooling_current_status_mutex);
    bool cooling_values_updated = false;

    /* Update cooling temperature */
    static unsigned long cooling_temp_last_update = 0;
    unsigned long current_time = millis();
    if (cooling_temp_last_update == 0) {
        cooling_temp_last_update = current_time;
    }

    unsigned long cooling_temp_delta_time = current_time - cooling_temp_last_update;
    if (cooling_temp_delta_time >= COOLING_TEMP_UPDATE_INTERVAL) {
        static float_t cooling_input_thermistor_buffer[COOLING_TEMP_BUFFER_LENGTH];
        static float_t cooling_output_thermistor_buffer[COOLING_TEMP_BUFFER_LENGTH];
        static uint32_t cooling_thermistor_buffer_index = 0;

        float_t input_thermistor_pin_voltage =
            (float_t)esp_adc_cal_raw_to_voltage(analogRead(PIN_COOLING_THERMISTOR_IN), adc_chars);

        float_t output_thermistor_pin_voltage =
            (float_t)esp_adc_cal_raw_to_voltage(analogRead(PIN_COOLING_THERMISTOR_OUT), adc_chars);

        // Add current thermistors resistances to the buffers
        cooling_input_thermistor_buffer[cooling_thermistor_buffer_index] =
            (input_thermistor_pin_voltage * COOLING_THERMISTOR_VOLTAGE_DIVIDER_RESISTANCE) /
            (COOLING_THERMISTOR_REF_VOLTAGE - input_thermistor_pin_voltage);
        cooling_output_thermistor_buffer[cooling_thermistor_buffer_index] =
            (output_thermistor_pin_voltage * COOLING_THERMISTOR_VOLTAGE_DIVIDER_RESISTANCE) /
            (COOLING_THERMISTOR_REF_VOLTAGE - input_thermistor_pin_voltage);
        cooling_thermistor_buffer_index++;

        // If the buffer is full compute the average temperature
        if (cooling_thermistor_buffer_index >= COOLING_TEMP_BUFFER_LENGTH) {
            // Reset buffer
            cooling_thermistor_buffer_index = 0;

            // Compute average buffer value
            float_t average_input_thermistor_value = 0;
            float_t average_output_thermistor_value = 0;
            int8_t valid_count = 0;
            for (uint32_t i = 0; i < COOLING_TEMP_BUFFER_LENGTH; i++) {
                average_input_thermistor_value += cooling_input_thermistor_buffer[i];
                average_output_thermistor_value += cooling_output_thermistor_buffer[i];
            }
            average_input_thermistor_value /= COOLING_TEMP_BUFFER_LENGTH;
            average_output_thermistor_value /= COOLING_TEMP_BUFFER_LENGTH;

            // Seinhart-Hart equation
            cooling_values.input_temperature = cooling_seinhart_hart_temperature(average_input_thermistor_value);
            cooling_values.output_temperature = cooling_seinhart_hart_temperature(average_output_thermistor_value);
            cooling_values_updated = true;
        }

        cooling_temp_last_update = current_time;
    }

    // Update cooling flow
    static unsigned long cooling_flow_last_update = 0;
    if (cooling_flow_last_update == 0) {
        cooling_flow_last_update = current_time;
    }

    unsigned long cooling_flow_delta_time = current_time - cooling_flow_last_update;
    if (cooling_flow_delta_time >= COOLING_FLOW_UPDATE_INTERVAL) {
        // F = 11 * Q ± 5% , Q = L/min
        cooling_values.input_flow =
            ((float_t)cooling_flow_input_interrupt_counter / 11) / ((float_t)cooling_flow_delta_time / 1000);
        cooling_values.output_flow =
            ((float_t)cooling_flow_output_interrupt_counter / 11) / ((float_t)cooling_flow_delta_time / 1000);
        cooling_values_updated = true;

        // Reset counter
        cooling_flow_input_interrupt_counter = 0;
        cooling_flow_output_interrupt_counter = 0;
        cooling_flow_last_update = current_time;
    }

    // Send update to the queue
    if (cooling_values_updated) {
        // Change alert state
        bool enable_alert =
            (cooling_values.input_temperature < probes_settings.cooling_temp_min ||
             cooling_values.input_temperature > probes_settings.cooling_temp_max ||
             cooling_values.input_flow < probes_settings.cooling_flow_min ||
             cooling_values.input_flow > probes_settings.cooling_temp_max ||
             cooling_values.output_temperature < probes_settings.cooling_temp_min ||
             cooling_values.output_temperature > probes_settings.cooling_temp_max ||
             cooling_values.output_flow < probes_settings.cooling_flow_min ||
             cooling_values.output_flow > probes_settings.cooling_temp_max);

        alerts_toggle_alert(ALERT_TYPE_COOLING, enable_alert);

#if HAS_DISPLAY
        // Notify UI of new values
        ui_status_notify_update(STATUS_UPDATE_PROBE_COOLING);
#else
        headless_notify_update(HEADLESS_UPDATE_STATUS);
#endif
    }

    RELEASE_MUTEX(cooling_current_status_mutex);
}
