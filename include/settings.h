#ifndef SETTINGS_H
#define SETTINGS_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <math.h>

#include "K40/bed.h"

#define SETTINGS_UPDATE_INTERVAL 1000

typedef enum {
    SETTINGS_TYPE_BED = 0x01,
    SETTINGS_TYPE_PROBES = 0x02,
    SETTINGS_TYPE_OTA = 0x04,
} SettingsType;

typedef struct {
    uint32_t screw_lead_um;
    uint32_t microstep_multiplier;
    uint32_t steps_per_revolution;
    uint32_t moving_speed;
    uint32_t homing_speed;
    BedPosition origin;
} BedSettings;

typedef struct {
    float_t cooling_flow_min;
    float_t cooling_flow_max;
    float_t cooling_temp_min;
    float_t cooling_temp_max;
} ProbesSettings;

typedef struct {
    char login[250];
    char password[250];
} OTASettings;

extern SemaphoreHandle_t bed_settings_mutex;
extern SemaphoreHandle_t probes_settings_mutex;
extern SemaphoreHandle_t ota_settings_mutex;

extern BedSettings bed_settings;
extern ProbesSettings probes_settings;
extern OTASettings ota_settings;

void settings_init();
void settings_schedule_save(uint32_t settings_types);

#endif
