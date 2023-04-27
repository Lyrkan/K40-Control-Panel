#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <Preferences.h>

#include "settings.h"

static TaskHandle_t save_settings_task;

static const char PREFERENCES_NAMESPACE_BED[] = "bed-settings";
static const char PREFERENCES_NAMESPACE_PROBES[] = "probes-settings";

static const char PREFERENCES_KEY_BED_SCREW_PITCH[] = "screw-pitch";
static const char PREFERENCES_KEY_BED_MICROSTEP_MULTIPLIER[] = "microstep-mul";
static const char PREFERENCES_KEY_BED_STEPS_PER_REVOLUTION[] = "steps-per-rev";
static const char PREFERENCES_KEY_BED_ACCELERATION[] = "acceleration";
static const char PREFERENCES_KEY_BED_MOVE_SPEED[] = "move-speed";
static const char PREFERENCES_KEY_BED_HOMING_SPEED[] = "homing-speed";
static const char PREFERENCES_KEY_BED_ORIGIN[] = "origin";

static const char PREFERENCES_KEY_PROBES_V1_MIN[] = "v1_min";
static const char PREFERENCES_KEY_PROBES_V1_MAX[] = "v1_max";
static const char PREFERENCES_KEY_PROBES_V2_MIN[] = "v2_min";
static const char PREFERENCES_KEY_PROBES_V2_MAX[] = "v2_max";
static const char PREFERENCES_KEY_PROBES_V3_MIN[] = "v3_min";
static const char PREFERENCES_KEY_PROBES_V3_MAX[] = "v3_max";
static const char PREFERENCES_KEY_PROBES_COOLING_FLOW_MIN[] = "cool_flow_min";
static const char PREFERENCES_KEY_PROBES_COOLING_FLOW_MAX[] = "cool_flow_max";
static const char PREFERENCES_KEY_PROBES_COOLING_TEMP_MIN[] = "cool_temp_min";
static const char PREFERENCES_KEY_PROBES_COOLING_TEMP_MAX[] = "cool_temp_max";

static Preferences preferences;

SemaphoreHandle_t bed_settings_mutex = xSemaphoreCreateMutex();
SemaphoreHandle_t probes_settings_mutex = xSemaphoreCreateMutex();

BedSettings bed_settings = {
    .screw_pitch = 0.8,
    .microstep_multiplier = 8,
    .steps_per_revolution = 200,
    .acceleration = 700,
    .moving_speed = 2500,
    .homing_speed = 1000,
    .origin = {.is_set = false, .position = 0}};

ProbesSettings probes_settings = {
    .voltage_probe_v1_min = 4.0,
    .voltage_probe_v1_max = 5.5,
    .voltage_probe_v2_min = 11.0,
    .voltage_probe_v2_max = 12.5,
    .voltage_probe_v3_min = 24.0,
    .voltage_probe_v3_max = 25.5,
    .cooling_flow_min = 1.0,
    .cooling_flow_max = 99.0,
    .cooling_temp_min = 5.0,
    .cooling_temp_max = 30.0,
};

static void settings_save_task(void *params) {
    uint32_t settings_types;

    while (true) {
        // Wait for a change notification
        xTaskNotifyWait(0x00, ULONG_MAX, &settings_types, portMAX_DELAY);
        Serial.println("Received a settings save request");

        if ((settings_types & SETTINGS_TYPE_BED) != 0) {
            Serial.println("Bed settings have changed, saving new values...");

            // Acquire bed settings lock
            while (xSemaphoreTake(bed_settings_mutex, portMAX_DELAY) != pdPASS)
                ;

            preferences.begin(PREFERENCES_NAMESPACE_BED, false);
            preferences.putFloat(PREFERENCES_KEY_BED_SCREW_PITCH, bed_settings.screw_pitch);
            preferences.putUInt(PREFERENCES_KEY_BED_MICROSTEP_MULTIPLIER, bed_settings.microstep_multiplier);
            preferences.putUInt(PREFERENCES_KEY_BED_STEPS_PER_REVOLUTION, bed_settings.steps_per_revolution);
            preferences.putUInt(PREFERENCES_KEY_BED_ACCELERATION, bed_settings.acceleration);
            preferences.putUInt(PREFERENCES_KEY_BED_MOVE_SPEED, bed_settings.moving_speed);
            preferences.putUInt(PREFERENCES_KEY_BED_HOMING_SPEED, bed_settings.homing_speed);
            if (bed_settings.origin.is_set) {
                preferences.putFloat(PREFERENCES_KEY_BED_ORIGIN, bed_settings.origin.position);
            }
            preferences.end();

            // Release bed settings lock
            xSemaphoreGive(bed_settings_mutex);
        }

        if ((settings_types & SETTINGS_TYPE_PROBES) != 0) {
            Serial.println("Probes settings have changed, saving new values...");

            // Acquire probes settings lock
            while (xSemaphoreTake(probes_settings_mutex, portMAX_DELAY) != pdPASS)
                ;

            preferences.begin(PREFERENCES_NAMESPACE_PROBES, false);
            preferences.putFloat(PREFERENCES_KEY_PROBES_V1_MIN, probes_settings.voltage_probe_v1_min);
            preferences.putFloat(PREFERENCES_KEY_PROBES_V1_MAX, probes_settings.voltage_probe_v1_max);
            preferences.putFloat(PREFERENCES_KEY_PROBES_V2_MIN, probes_settings.voltage_probe_v2_min);
            preferences.putFloat(PREFERENCES_KEY_PROBES_V2_MAX, probes_settings.voltage_probe_v2_max);
            preferences.putFloat(PREFERENCES_KEY_PROBES_V3_MIN, probes_settings.voltage_probe_v3_min);
            preferences.putFloat(PREFERENCES_KEY_PROBES_V3_MAX, probes_settings.voltage_probe_v3_max);
            preferences.putFloat(PREFERENCES_KEY_PROBES_COOLING_FLOW_MIN, probes_settings.cooling_flow_min);
            preferences.putFloat(PREFERENCES_KEY_PROBES_COOLING_FLOW_MAX, probes_settings.cooling_flow_max);
            preferences.putFloat(PREFERENCES_KEY_PROBES_COOLING_TEMP_MIN, probes_settings.cooling_temp_min);
            preferences.putFloat(PREFERENCES_KEY_PROBES_COOLING_TEMP_MAX, probes_settings.cooling_temp_max);
            preferences.end();

            // Release probes settings lock
            xSemaphoreGive(probes_settings_mutex);
        }

        // Wait a little bit before the next check
        delay(SETTINGS_UPDATE_INTERVAL);
    }
}

void settings_init() {
    // Acquire bed settings lock
    while (xSemaphoreTake(bed_settings_mutex, portMAX_DELAY) != pdPASS)
        ;

    Serial.println("Loading bed settings... ");
    preferences.begin(PREFERENCES_NAMESPACE_BED, true);

    // clang-format off
    bed_settings = {
        .screw_pitch = preferences.getFloat(PREFERENCES_KEY_BED_SCREW_PITCH, bed_settings.screw_pitch),
        .microstep_multiplier = preferences.getUInt(PREFERENCES_KEY_BED_MICROSTEP_MULTIPLIER, bed_settings.microstep_multiplier),
        .steps_per_revolution = preferences.getUInt(PREFERENCES_KEY_BED_STEPS_PER_REVOLUTION, bed_settings.steps_per_revolution),
        .acceleration = preferences.getUInt(PREFERENCES_KEY_BED_ACCELERATION, bed_settings.acceleration),
        .moving_speed = preferences.getUInt(PREFERENCES_KEY_BED_MOVE_SPEED, bed_settings.moving_speed),
        .homing_speed = preferences.getUInt(PREFERENCES_KEY_BED_HOMING_SPEED, bed_settings.homing_speed),
        .origin = {
            .is_set = preferences.isKey(PREFERENCES_KEY_BED_ORIGIN),
            .position = preferences.getFloat(PREFERENCES_KEY_BED_ORIGIN),
        }
    };
    // clang-format on

    preferences.end();

    // Release bed settings lock
    xSemaphoreGive(bed_settings_mutex);

    // Acquire probes settings lock
    while (xSemaphoreTake(probes_settings_mutex, portMAX_DELAY) != pdPASS)
        ;
    Serial.println("Loading probes settings... ");
    preferences.begin(PREFERENCES_NAMESPACE_PROBES, true);

    // clang-format off
    probes_settings = {
        .voltage_probe_v1_min = preferences.getFloat(PREFERENCES_KEY_PROBES_V1_MIN, probes_settings.voltage_probe_v1_min),
        .voltage_probe_v1_max = preferences.getFloat(PREFERENCES_KEY_PROBES_V1_MAX, probes_settings.voltage_probe_v1_max),
        .voltage_probe_v2_min = preferences.getFloat(PREFERENCES_KEY_PROBES_V2_MIN, probes_settings.voltage_probe_v2_min),
        .voltage_probe_v2_max = preferences.getFloat(PREFERENCES_KEY_PROBES_V2_MAX, probes_settings.voltage_probe_v2_max),
        .voltage_probe_v3_min = preferences.getFloat(PREFERENCES_KEY_PROBES_V3_MIN, probes_settings.voltage_probe_v3_min),
        .voltage_probe_v3_max = preferences.getFloat(PREFERENCES_KEY_PROBES_V3_MAX, probes_settings.voltage_probe_v3_max),
        .cooling_flow_min = preferences.getFloat(PREFERENCES_KEY_PROBES_COOLING_FLOW_MIN, probes_settings.cooling_flow_min),
        .cooling_flow_max = preferences.getFloat(PREFERENCES_KEY_PROBES_COOLING_FLOW_MAX, probes_settings.cooling_flow_max),
        .cooling_temp_min = preferences.getFloat(PREFERENCES_KEY_PROBES_COOLING_TEMP_MIN, probes_settings.cooling_temp_min),
        .cooling_temp_max = preferences.getFloat(PREFERENCES_KEY_PROBES_COOLING_TEMP_MAX, probes_settings.cooling_temp_max),
    };
    // clang-format on

    preferences.end();

    // Release probes settings lock
    xSemaphoreGive(probes_settings_mutex);

    // Start saving task
    xTaskCreatePinnedToCore(
        settings_save_task,
        "SettingsSaveTask",
        10000,
        NULL,
        0,
        &save_settings_task,
        1 // Run on core #1, UI will also be updated by loop() in core#1
    );
}

void settings_schedule_save(uint32_t settings_types) { xTaskNotify(save_settings_task, settings_types, eSetBits); }
