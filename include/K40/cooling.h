#ifndef K40_COOLING_H
#define K40_COOLING_H

#include <Arduino.h>
#include <math.h>
#include "esp_adc_cal.h"

#define COOLING_THERMISTOR_NOMINAL_TEMPERATURE 25
#define COOLING_THERMISTOR_MINIMUM_TEMPERATURE -2.5
#define COOLING_THERMISTOR_NOMINAL_RESISTANCE 50000
#define COOLING_THERMISTOR_REF_VOLTAGE 3300
#define COOLING_THERMISTOR_COEFF 3950
#define COOLING_THERMISTOR_VOLTAGE_DIVIDER_RESISTANCE 10000

#define COOLING_FLOW_UPDATE_INTERVAL 500 // ms
#define COOLING_TEMP_UPDATE_INTERVAL 25  // ms
#define COOLING_TEMP_BUFFER_LENGTH 100

typedef enum {
    PIN_COOLING_THERMISTOR_IN = 39,
    PIN_COOLING_FLOW_IN = 25,
    PIN_COOLING_THERMISTOR_OUT = 36,
    PIN_COOLING_FLOW_OUT = 33
} CoolingPin;

typedef struct {
    float_t input_flow;
    float_t input_temperature;
    float_t output_flow;
    float_t output_temperature;
} CoolingValues;

extern CoolingValues cooling_values;

void IRAM_ATTR cooling_flow_input_probe_interrupt();
void IRAM_ATTR cooling_flow_output_probe_interrupt();
void cooling_update_status(esp_adc_cal_characteristics_t *adc_chars);

#endif
