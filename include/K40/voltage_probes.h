#ifndef K40_VOLTAGE_PROBES_H
#define K40_VOLTAGE_PROBES_H

#include <Arduino.h>
#include <math.h>
#include "esp_adc_cal.h"

#define VOLTAGE_PROBES_UPDATE_INTERVAL 1000 // ms

#define VOLTAGE_PROBE_1_MIN_VALUE 4.0
#define VOLTAGE_PROBE_2_MIN_VALUE 11.0
#define VOLTAGE_PROBE_3_MIN_VALUE 24.0

#define VOLTAGE_PROBE_1_MAX_VALUE 5.5
#define VOLTAGE_PROBE_2_MAX_VALUE 12.5
#define VOLTAGE_PROBE_3_MAX_VALUE 25.5

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
