#ifndef K40_VOLTAGE_PROBES_H
#define K40_VOLTAGE_PROBES_H

#include <Arduino.h>
#include <math.h>
#include "esp_adc_cal.h"

#define VOLTAGE_PROBES_UPDATE_INTERVAL 1000 // ms

enum VoltageProbePin {
    PIN_VOLTAGE_PROBE_1 = 34,
    PIN_VOLTAGE_PROBE_2 = 39,
    PIN_VOLTAGE_PROBE_3 = 36
};

typedef struct VoltageProbesValues VoltageProbesValues;
struct VoltageProbesValues {
    float_t probe1;
    float_t probe2;
    float_t probe3;
};

void voltage_probes_update_status(esp_adc_cal_characteristics_t *adc_chars);

#endif
