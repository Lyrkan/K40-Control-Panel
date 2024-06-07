#ifndef SETTINGS_H
#define SETTINGS_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <math.h>
#include "LGFX/LGFX.h"

#include "K40/bed.h"

#define SETTINGS_UPDATE_INTERVAL 1000

typedef enum {
    SETTINGS_TYPE_BED = 1 << 0,
    SETTINGS_TYPE_PROBES = 1 << 1,
    SETTINGS_TYPE_OTA = 1 << 2,
    SETTINGS_TYPE_GRBL = 1 << 3,
    SETTINGS_TYPE_RELAYS = 1 << 4,
} SettingsType;

typedef struct {
    uint32_t screw_lead_um;
    uint32_t microstep_multiplier;
    uint32_t steps_per_revolution;
    uint32_t moving_speed;
    uint32_t homing_speed;
    BedPosition origin;
    uint32_t backoff_distance_um;
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

typedef struct {
    float_t jog_speed;
    uint32_t default_timeout_ms;
    uint32_t homing_timeout_ms;
} GrblSettings;

typedef struct {
    uint32_t alarm_behavior;
    uint32_t interlock_behavior;
} RelaysSettings;

typedef enum {
    ALARM_ENABLE_WHEN_RUNNING = 1 << 0,
    ALARM_ENABLE_WHEN_NOT_IDLING = 1 << 1,
    ALARM_ENABLE_WHEN_FLAME_SENSOR_TRIGGERED = 1 << 2,
    ALARM_ENABLE_WHEN_COOLING_ISSUE = 1 << 3,
    ALARM_ENABLE_WHEN_LID_OPENED = 1 << 4
} AlarmBehaviorFlag;

typedef enum {
    INTERLOCK_DISABLE_WHEN_LID_OPENED = 1 << 0,
    INTERLOCK_DISABLE_WHEN_COOLING_ISSUE = 1 << 1,
    INTERLOCK_DISABLE_WHEN_FLAME_SENSOR_TRIGGERED = 1 << 2,
} InterlockBehaviorFlag;

extern SemaphoreHandle_t bed_settings_mutex;
extern SemaphoreHandle_t probes_settings_mutex;
extern SemaphoreHandle_t ota_settings_mutex;
extern SemaphoreHandle_t grbl_settings_mutex;
extern SemaphoreHandle_t relays_settings_mutex;

extern BedSettings bed_settings;
extern ProbesSettings probes_settings;
extern OTASettings ota_settings;
extern GrblSettings grbl_settings;
extern RelaysSettings relays_settings;

void settings_init();
void settings_schedule_save(uint32_t settings_types);
void settings_load_touchscreen_calibration_data(LGFX *tft);

#endif
