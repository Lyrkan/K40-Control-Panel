#ifndef K40_COOLING_H
#define K40_COOLING_H

#include <Arduino.h>
#include <math.h>
#include "esp_adc_cal.h"

#define COOLING_THERMISTOR_NOMINAL_TEMPERATURE 25
#define COOLING_THERMISTOR_NOMINAL_RESISTANCE 50000
#define COOLING_THERMISTOR_REF_VOLTAGE 3300
#define COOLING_THERMISTOR_COEFF 3950
#define COOLING_THERMISTOR_VOLTAGE_DIVIDER_RESISTANCE 10000
#define COOLING_THERMISTOR_BUFFER_LENGTH 50

#define COOLING_TEMP_MIN_VALUE 5
#define COOLING_TEMP_MAX_VALUE 30
#define COOLING_FLOW_MIN_VALUE 1
#define COOLING_FLOW_MAX_VALUE 99

#define COOLING_FLOW_UPDATE_INTERVAL 500 // ms

enum CoolingPin {
    PIN_COOLING_THERMISTOR = 33,
    PIN_COOLING_FLOW = 25
};

typedef struct CoolingValues CoolingValues;
struct CoolingValues {
    float_t flow;
    float_t temperature;
};

void IRAM_ATTR cooling_flow_probe_interrupt();
void cooling_update_status(esp_adc_cal_characteristics_t *adc_chars);

#endif
