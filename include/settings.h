#ifndef SETTINGS_H
#define SETTINGS_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <math.h>

#include "K40/bed.h"

#define SETTINGS_UPDATE_INTERVAL 1000

enum SettingsType {
    SETTINGS_TYPE_BED = 0x01,
    SETTINGS_TYPE_PROBES = 0x02,
    SETTINGS_TYPE_OTA = 0x04,
};

typedef struct BedSettings BedSettings;
struct BedSettings {
    uint32_t screw_pitch_um;
    uint32_t microstep_multiplier;
    uint32_t steps_per_revolution;
    uint32_t moving_speed;
    uint32_t homing_speed;
    BedPosition origin;
};

typedef struct ProbesSettings ProbesSettings;
struct ProbesSettings {
    float_t voltage_probe_v1_min;
    float_t voltage_probe_v1_max;
    float_t voltage_probe_v2_min;
    float_t voltage_probe_v2_max;
    float_t voltage_probe_v3_min;
    float_t voltage_probe_v3_max;

    float_t cooling_flow_min;
    float_t cooling_flow_max;
    float_t cooling_temp_min;
    float_t cooling_temp_max;
};

typedef struct OTASettings OTASettings;
struct OTASettings {
    char login[250];
    char password[250];
};

extern SemaphoreHandle_t bed_settings_mutex;
extern SemaphoreHandle_t probes_settings_mutex;
extern SemaphoreHandle_t ota_settings_mutex;

extern BedSettings bed_settings;
extern ProbesSettings probes_settings;
extern OTASettings ota_settings;

void settings_init();
void settings_schedule_save(uint32_t settings_types);

#endif
