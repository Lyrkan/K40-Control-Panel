#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <math.h>
#include "esp_adc_cal.h"

#include "K40/cooling.h"
#include "UI/screens/status.h"
#include "queues.h"

static CoolingValues cooling_values;

static uint32_t cooling_flow_interrupt_counter = 0;
void IRAM_ATTR cooling_flow_probe_interrupt() { cooling_flow_interrupt_counter++; }

void cooling_update_status(esp_adc_cal_characteristics_t *adc_chars) {
    bool cooling_values_updated = false;

    /* Update cooling temperature */
    static float_t cooling_thermistor_buffer[COOLING_THERMISTOR_BUFFER_LENGTH];
    static uint32_t cooling_thermistor_buffer_index = 0;

    float_t thermistor_pin_voltage = (float_t)esp_adc_cal_raw_to_voltage(analogRead(PIN_COOLING_THERMISTOR), adc_chars);

    // Add current thermistor resistance to the buffer
    cooling_thermistor_buffer[cooling_thermistor_buffer_index] =
        (thermistor_pin_voltage * COOLING_THERMISTOR_VOLTAGE_DIVIDER_RESISTANCE) /
        (COOLING_THERMISTOR_REF_VOLTAGE - thermistor_pin_voltage);
    cooling_thermistor_buffer_index++;

    // If the buffer is full compute the average temperature
    if (cooling_thermistor_buffer_index >= COOLING_THERMISTOR_BUFFER_LENGTH) {
        // Reset buffer
        cooling_thermistor_buffer_index = 0;

        // Compute average buffer value
        float_t average_thermistor_value = 0;
        for (uint32_t i = 0; i < COOLING_THERMISTOR_BUFFER_LENGTH; i++) {
            average_thermistor_value += cooling_thermistor_buffer[cooling_thermistor_buffer_index];
        }
        average_thermistor_value /= COOLING_THERMISTOR_BUFFER_LENGTH;

        // Seinhart-Hart equation
        float_t cooling_temperature = log(average_thermistor_value / COOLING_THERMISTOR_NOMINAL_RESISTANCE);
        cooling_temperature /= COOLING_THERMISTOR_COEFF;
        cooling_temperature += 1.0 / (COOLING_THERMISTOR_NOMINAL_TEMPERATURE + 273.15);
        cooling_temperature = 1.0 / cooling_temperature;
        cooling_temperature -= 273.15;

        cooling_values.temperature = cooling_temperature;
        cooling_values_updated = true;
    }

    // Update cooling flow
    static unsigned long cooling_flow_last_update = 0;
    unsigned long current_time = millis();
    if (cooling_flow_last_update == 0) {
        cooling_flow_last_update = current_time;
    }

    unsigned long delta_time = current_time - cooling_flow_last_update;
    if (delta_time >= COOLING_FLOW_UPDATE_INTERVAL) {
        // F = 11 * Q ± 5% , Q = L/min
        cooling_values.flow = (cooling_flow_interrupt_counter / 11) / ((float_t)delta_time / 1000);
        cooling_values_updated = true;

        // Reset counter
        cooling_flow_interrupt_counter = 0;
        cooling_flow_last_update = current_time;
    }

    // Send update to the queue
    if (cooling_values_updated) {
        xQueueOverwrite(cooling_current_status_queue, &cooling_values);
        ui_status_notify_update(STATUS_UPDATE_PROBE_COOLING);
    }
}
