#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#if HAS_DISPLAY
#include "LGFX/LGFX.h"
#endif

#include "macros.h"
#include "settings.h"
#include "tasks.h"

static const char PREFERENCES_NAMESPACE_BED[] = "bed-settings";
static const char PREFERENCES_NAMESPACE_PROBES[] = "probes-settings";
static const char PREFERENCES_NAMESPACE_OTA[] = "ota-settings";
static const char PREFERENCES_NAMESPACE_GRBL[] = "grbl-settings";
static const char PREFERENCES_NAMESPACE_RELAYS[] = "relays-settings";
static const char PREFERENCES_NAMESPACE_SCREEN[] = "screen-settings";

static const char PREFERENCES_KEY_BED_CONTROL_MODE[] = "control-mode";
static const char PREFERENCES_KEY_BED_SCREW_LEAD[] = "screw-lead-um";
static const char PREFERENCES_KEY_BED_MICROSTEP_MULTIPLIER[] = "microstep-mul";
static const char PREFERENCES_KEY_BED_STEPS_PER_REVOLUTION[] = "steps-per-rev";
static const char PREFERENCES_KEY_BED_MOVE_SPEED[] = "move-speed";
static const char PREFERENCES_KEY_BED_HOMING_SPEED[] = "homing-speed";
static const char PREFERENCES_KEY_BED_ORIGIN[] = "origin";
static const char PREFERENCES_KEY_BED_BACKOFF_DISTANCE_UM[] = "backoff-dist-um";

static const char PREFERENCES_KEY_PROBES_COOLING_FLOW_MIN[] = "cool-flow-min";
static const char PREFERENCES_KEY_PROBES_COOLING_FLOW_MAX[] = "cool-flow-max";
static const char PREFERENCES_KEY_PROBES_COOLING_TEMP_MIN[] = "cool-temp-min";
static const char PREFERENCES_KEY_PROBES_COOLING_TEMP_MAX[] = "cool-temp-max";

static const char PREFERENCES_KEY_OTA_LOGIN[] = "login";
static const char PREFERENCES_KEY_OTA_PASSWORD[] = "password";

static const char PREFERENCES_KEY_GRBL_JOG_SPEED[] = "jog-speed";
static const char PREFERENCES_KEY_GRBL_DEFAULT_TIMEOUT[] = "default-timeout";
static const char PREFERENCES_KEY_GRBL_HOMING_TIMEOUT[] = "homing-timeout";

static const char PREFERENCES_KEY_RELAYS_ALARM_BEHAVIOR[] = "alarm-flags";
static const char PREFERENCES_KEY_RELAYS_INTERLOCK_BEHAVIOR[] = "interlock-flags";

static const char PREFERENCES_KEY_SCREEN_TOUCH_CALIBRATION[] = "touch-calib";

static Preferences preferences;

SemaphoreHandle_t bed_settings_mutex = xSemaphoreCreateMutex();
SemaphoreHandle_t probes_settings_mutex = xSemaphoreCreateMutex();
SemaphoreHandle_t ota_settings_mutex = xSemaphoreCreateMutex();
SemaphoreHandle_t grbl_settings_mutex = xSemaphoreCreateMutex();
SemaphoreHandle_t relays_settings_mutex = xSemaphoreCreateMutex();

BedSettings bed_settings = {
    .control_mode = BED_CONTROL_MODE_STEPPER,
    .screw_lead_um = 8000,
    .microstep_multiplier = 8,
    .steps_per_revolution = 200,
    .moving_speed = 4500,
    .homing_speed = 3000,
    .origin = {.is_set = false, .position_nm = 0},
    .backoff_distance_um = 3000};

ProbesSettings probes_settings = {
    .cooling_flow_min = 1.0,
    .cooling_flow_max = 99.0,
    .cooling_temp_min = 5.0,
    .cooling_temp_max = 30.0,
};

OTASettings ota_settings = {
    .login = {0},
    .password = {0},
};

GrblSettings grbl_settings = {
    .jog_speed = 100.0,
    .default_timeout_ms = 5000,
    .homing_timeout_ms = 30000,
};

RelaysSettings relays_settings = {
    .alarm_behavior =
        ALARM_ENABLE_WHEN_COOLING_ISSUE | ALARM_ENABLE_WHEN_FLAME_SENSOR_TRIGGERED | ALARM_ENABLE_WHEN_LID_OPENED,
    .interlock_behavior = INTERLOCK_DISABLE_WHEN_COOLING_ISSUE | INTERLOCK_DISABLE_WHEN_LID_OPENED,
};

static void settings_save_task_func(void *params) {
    uint32_t settings_types;

    while (true) {
        // Wait for a change notification
        xTaskNotifyWait(0x00, ULONG_MAX, &settings_types, portMAX_DELAY);
        log_i("Received a settings save request");

        if ((settings_types & SETTINGS_TYPE_BED) != 0) {
            log_i("Bed settings have changed, saving new values...");

            // Acquire bed settings lock
            TAKE_MUTEX(bed_settings_mutex)

            preferences.begin(PREFERENCES_NAMESPACE_BED, false);
            preferences.putUInt(PREFERENCES_KEY_BED_CONTROL_MODE, bed_settings.control_mode);
            preferences.putUInt(PREFERENCES_KEY_BED_SCREW_LEAD, bed_settings.screw_lead_um);
            preferences.putUInt(PREFERENCES_KEY_BED_MICROSTEP_MULTIPLIER, bed_settings.microstep_multiplier);
            preferences.putUInt(PREFERENCES_KEY_BED_STEPS_PER_REVOLUTION, bed_settings.steps_per_revolution);
            preferences.putUInt(PREFERENCES_KEY_BED_MOVE_SPEED, bed_settings.moving_speed);
            preferences.putUInt(PREFERENCES_KEY_BED_HOMING_SPEED, bed_settings.homing_speed);
            if (bed_settings.origin.is_set) {
                preferences.putInt(PREFERENCES_KEY_BED_ORIGIN, bed_settings.origin.position_nm);
            }
            preferences.putUInt(PREFERENCES_KEY_BED_BACKOFF_DISTANCE_UM, bed_settings.backoff_distance_um);
            preferences.end();

            // Release bed settings lock
            RELEASE_MUTEX(bed_settings_mutex)
        }

        if ((settings_types & SETTINGS_TYPE_PROBES) != 0) {
            log_i("Probes settings have changed, saving new values...");

            // Acquire probes settings lock
            TAKE_MUTEX(probes_settings_mutex)

            preferences.begin(PREFERENCES_NAMESPACE_PROBES, false);
            preferences.putFloat(PREFERENCES_KEY_PROBES_COOLING_FLOW_MIN, probes_settings.cooling_flow_min);
            preferences.putFloat(PREFERENCES_KEY_PROBES_COOLING_FLOW_MAX, probes_settings.cooling_flow_max);
            preferences.putFloat(PREFERENCES_KEY_PROBES_COOLING_TEMP_MIN, probes_settings.cooling_temp_min);
            preferences.putFloat(PREFERENCES_KEY_PROBES_COOLING_TEMP_MAX, probes_settings.cooling_temp_max);
            preferences.end();

            // Release probes settings lock
            RELEASE_MUTEX(probes_settings_mutex)
        }

        if ((settings_types & SETTINGS_TYPE_OTA) != 0) {
            log_i("OTA settings have changed, saving new values...");

            // Acquire OTA settings lock
            TAKE_MUTEX(ota_settings_mutex)

            preferences.begin(PREFERENCES_NAMESPACE_OTA, false);
            preferences.putString(PREFERENCES_KEY_OTA_LOGIN, ota_settings.login);
            preferences.putString(PREFERENCES_KEY_OTA_PASSWORD, ota_settings.password);
            preferences.end();

            // Release OTA settings lock
            RELEASE_MUTEX(ota_settings_mutex)
        }

        if ((settings_types & SETTINGS_TYPE_GRBL) != 0) {
            log_i("GRBL settings have changed, saving new values...");

            // Acquire GRBL settings lock
            TAKE_MUTEX(grbl_settings_mutex)

            preferences.begin(PREFERENCES_NAMESPACE_GRBL, false);
            preferences.putFloat(PREFERENCES_KEY_GRBL_JOG_SPEED, grbl_settings.jog_speed);
            preferences.putUInt(PREFERENCES_KEY_GRBL_DEFAULT_TIMEOUT, grbl_settings.default_timeout_ms);
            preferences.putUInt(PREFERENCES_KEY_GRBL_HOMING_TIMEOUT, grbl_settings.homing_timeout_ms);
            preferences.end();

            // Release GRBL settings lock
            RELEASE_MUTEX(grbl_settings_mutex)
        }

        if ((settings_types & SETTINGS_TYPE_RELAYS) != 0) {
            log_i("Relays settings have changed, saving new values...");

            // Acquire relays settings lock
            TAKE_MUTEX(relays_settings_mutex)

            preferences.begin(PREFERENCES_NAMESPACE_RELAYS, false);
            preferences.putUInt(PREFERENCES_KEY_RELAYS_ALARM_BEHAVIOR, relays_settings.alarm_behavior);
            preferences.putUInt(PREFERENCES_KEY_RELAYS_INTERLOCK_BEHAVIOR, relays_settings.interlock_behavior);
            preferences.end();

            // Release relays settings lock
            RELEASE_MUTEX(relays_settings_mutex)
        }

        // Wait a little bit before the next check
        vTaskDelay(pdMS_TO_TICKS(SETTINGS_UPDATE_INTERVAL));
    }
}

void settings_init() {
    // Acquire bed settings lock
    TAKE_MUTEX(bed_settings_mutex)

    log_i("Loading bed settings... ");
    preferences.begin(PREFERENCES_NAMESPACE_BED, true);

    // clang-format off
    bed_settings = {
        .control_mode = static_cast<BedControlMode>(preferences.getUInt(PREFERENCES_KEY_BED_CONTROL_MODE, bed_settings.control_mode)),
        .screw_lead_um = preferences.getUInt(PREFERENCES_KEY_BED_SCREW_LEAD, bed_settings.screw_lead_um),
        .microstep_multiplier = preferences.getUInt(PREFERENCES_KEY_BED_MICROSTEP_MULTIPLIER, bed_settings.microstep_multiplier),
        .steps_per_revolution = preferences.getUInt(PREFERENCES_KEY_BED_STEPS_PER_REVOLUTION, bed_settings.steps_per_revolution),
        .moving_speed = preferences.getUInt(PREFERENCES_KEY_BED_MOVE_SPEED, bed_settings.moving_speed),
        .homing_speed = preferences.getUInt(PREFERENCES_KEY_BED_HOMING_SPEED, bed_settings.homing_speed),
        .origin = {
            .is_set = preferences.isKey(PREFERENCES_KEY_BED_ORIGIN),
            .position_nm = preferences.getInt(PREFERENCES_KEY_BED_ORIGIN),
        },
        .backoff_distance_um = preferences.getUInt(PREFERENCES_KEY_BED_BACKOFF_DISTANCE_UM, bed_settings.backoff_distance_um),
    };
    // clang-format on

    preferences.end();

    log_d("  control_mode: %d", bed_settings.control_mode);
    log_d("  screw_lead_um: %d", bed_settings.screw_lead_um);
    log_d("  microstep_multiplier: %d", bed_settings.microstep_multiplier);
    log_d("  steps_per_revolution: %d", bed_settings.steps_per_revolution);
    log_d("  moving_speed: %d", bed_settings.moving_speed);
    log_d("  homing_speed: %d", bed_settings.homing_speed);
    log_d("  origin:");
    log_d("    is_set: %d", bed_settings.origin.is_set);
    log_d("    position_nm: %d", bed_settings.origin.position_nm);
    log_d("  backoff_distance_um: %d", bed_settings.backoff_distance_um);

    // Release bed settings lock
    RELEASE_MUTEX(bed_settings_mutex)

    // Acquire probes settings lock
    TAKE_MUTEX(probes_settings_mutex)
    log_i("Loading probes settings... ");
    preferences.begin(PREFERENCES_NAMESPACE_PROBES, true);

    // clang-format off
    probes_settings = {
        .cooling_flow_min = preferences.getFloat(PREFERENCES_KEY_PROBES_COOLING_FLOW_MIN, probes_settings.cooling_flow_min),
        .cooling_flow_max = preferences.getFloat(PREFERENCES_KEY_PROBES_COOLING_FLOW_MAX, probes_settings.cooling_flow_max),
        .cooling_temp_min = preferences.getFloat(PREFERENCES_KEY_PROBES_COOLING_TEMP_MIN, probes_settings.cooling_temp_min),
        .cooling_temp_max = preferences.getFloat(PREFERENCES_KEY_PROBES_COOLING_TEMP_MAX, probes_settings.cooling_temp_max),
    };
    // clang-format on

    preferences.end();

    log_d("  cooling_flow_min: %.4f", probes_settings.cooling_flow_min);
    log_d("  cooling_flow_max: %.4f", probes_settings.cooling_flow_max);
    log_d("  cooling_temp_min: %.4f", probes_settings.cooling_temp_min);
    log_d("  cooling_temp_max: %.4f", probes_settings.cooling_temp_max);

    // Release probes settings lock
    RELEASE_MUTEX(probes_settings_mutex)

    // Acquire OTA settings lock
    TAKE_MUTEX(ota_settings_mutex)
    log_i("Loading OTA settings... ");
    preferences.begin(PREFERENCES_NAMESPACE_OTA, true);

    strlcpy(
        ota_settings.login,
        preferences.getString(PREFERENCES_KEY_OTA_LOGIN).c_str(),
        ARRAY_SIZE(ota_settings.login));
    strlcpy(
        ota_settings.password,
        preferences.getString(PREFERENCES_KEY_OTA_PASSWORD).c_str(),
        ARRAY_SIZE(ota_settings.password));

    preferences.end();

    log_d("  login: %s", ota_settings.login);
    log_d("  password: %s", ota_settings.password);

    // Release OTA settings lock
    RELEASE_MUTEX(ota_settings_mutex)

    // Acquire GRBL settings lock
    TAKE_MUTEX(grbl_settings_mutex)
    log_i("Loading GRBL settings... ");
    preferences.begin(PREFERENCES_NAMESPACE_GRBL, true);

    // clang-format off
    grbl_settings = {
        .jog_speed = preferences.getFloat(PREFERENCES_KEY_GRBL_JOG_SPEED, grbl_settings.jog_speed),
        .default_timeout_ms = preferences.getUInt(PREFERENCES_KEY_GRBL_DEFAULT_TIMEOUT, grbl_settings.default_timeout_ms),
        .homing_timeout_ms = preferences.getUInt(PREFERENCES_KEY_GRBL_HOMING_TIMEOUT, grbl_settings.homing_timeout_ms),
    };
    //  clang-format on

    preferences.end();

    log_d("  jog speed: %.1f", grbl_settings.jog_speed);
    log_d("  default timeout: %d", grbl_settings.default_timeout_ms);
    log_d("  homing timeout: %d", grbl_settings.homing_timeout_ms);

    // Release GRBL settings lock
    RELEASE_MUTEX(grbl_settings_mutex)

    // Acquire relays settings lock
    TAKE_MUTEX(relays_settings_mutex)
    log_i("Loading relays settings... ");
    preferences.begin(PREFERENCES_NAMESPACE_RELAYS, true);

    // clang-format off
    relays_settings = {
        .alarm_behavior = preferences.getUInt(PREFERENCES_KEY_RELAYS_ALARM_BEHAVIOR, relays_settings.alarm_behavior),
        .interlock_behavior = preferences.getUInt(PREFERENCES_KEY_RELAYS_INTERLOCK_BEHAVIOR, relays_settings.interlock_behavior),
    };
    //  clang-format on

    preferences.end();

    log_d("  alarm behavior: 0x%x", relays_settings.alarm_behavior);
    log_d("  interlock behavior: 0x%x", relays_settings.interlock_behavior);

    // Release relays settings lock
    RELEASE_MUTEX(relays_settings_mutex)

    // Start saving task
    xTaskCreatePinnedToCore(
        settings_save_task_func,
        "settings_save",
        TASK_SETTINGS_SAVE_STACK_SIZE,
        NULL,
        TASK_SETTINGS_PRIORITY,
        &settings_save_task_handle,
        TASK_SETTINGS_CORE_ID);
}

void settings_schedule_save(uint32_t settings_types) {
    xTaskNotify(settings_save_task_handle, settings_types, eSetBits);
}

#if HAS_DISPLAY
void settings_load_touchscreen_calibration_data(LGFX *tft) {
    uint16_t touchscreen_calibration_data[8];

    preferences.begin(PREFERENCES_NAMESPACE_SCREEN, false);
    if (preferences.isKey(PREFERENCES_KEY_SCREEN_TOUCH_CALIBRATION)) {
        log_i("Loading touchscreen calibration data from settings...");
        preferences.getBytes(PREFERENCES_KEY_SCREEN_TOUCH_CALIBRATION, touchscreen_calibration_data, sizeof(touchscreen_calibration_data));
        tft->setTouchCalibrate(touchscreen_calibration_data);
    } else {
        log_i("No touchscreen calibration data found (first boot?), starting calibration procedure");
        tft->drawString("Touchscreen calibration: please touch corners in the requested order", 20, (tft->height()  / 2));
        tft->calibrateTouch(touchscreen_calibration_data, 0xFFFFFFU, 0x000000U, 20);
        preferences.putBytes(PREFERENCES_KEY_SCREEN_TOUCH_CALIBRATION, touchscreen_calibration_data, sizeof(touchscreen_calibration_data) );
    }
    preferences.end();
}
#endif

void settings_get_json(JsonDocument& doc) {
    // Bed settings
    TAKE_MUTEX(bed_settings_mutex)
    doc["bed"]["control_mode"] = bed_settings.control_mode;
    doc["bed"]["screw_lead_um"] = bed_settings.screw_lead_um;
    doc["bed"]["microstep_multiplier"] = bed_settings.microstep_multiplier;
    doc["bed"]["steps_per_revolution"] = bed_settings.steps_per_revolution;
    doc["bed"]["moving_speed"] = bed_settings.moving_speed;
    doc["bed"]["homing_speed"] = bed_settings.homing_speed;
    doc["bed"]["origin"]["is_set"] = bed_settings.origin.is_set;
    doc["bed"]["origin"]["position_nm"] = bed_settings.origin.position_nm;
    doc["bed"]["backoff_distance_um"] = bed_settings.backoff_distance_um;
    RELEASE_MUTEX(bed_settings_mutex)

    // Probes settings
    TAKE_MUTEX(probes_settings_mutex)
    doc["probes"]["cooling"]["flow"]["min"] = probes_settings.cooling_flow_min;
    doc["probes"]["cooling"]["flow"]["max"] = probes_settings.cooling_flow_max;
    doc["probes"]["cooling"]["temp"]["min"] = probes_settings.cooling_temp_min;
    doc["probes"]["cooling"]["temp"]["max"] = probes_settings.cooling_temp_max;
    RELEASE_MUTEX(probes_settings_mutex)

    // OTA settings
    TAKE_MUTEX(ota_settings_mutex)
    doc["ota"]["login"] = ota_settings.login;
    doc["ota"]["password"] = ota_settings.password;
    RELEASE_MUTEX(ota_settings_mutex)

    // GRBL settings
    TAKE_MUTEX(grbl_settings_mutex)
    doc["grbl"]["jog_speed"] = grbl_settings.jog_speed;
    doc["grbl"]["default_timeout_ms"] = grbl_settings.default_timeout_ms;
    doc["grbl"]["homing_timeout_ms"] = grbl_settings.homing_timeout_ms;
    RELEASE_MUTEX(grbl_settings_mutex)

    // Relays settings
    TAKE_MUTEX(relays_settings_mutex)
    doc["relays"]["alarm_behavior"] = relays_settings.alarm_behavior;
    doc["relays"]["interlock_behavior"] = relays_settings.interlock_behavior;
    RELEASE_MUTEX(relays_settings_mutex)
}

void settings_update_from_json(const JsonObject& doc) {
    uint32_t updated_settings_types = 0;

    // Bed settings
    if (doc.containsKey("bed")) {
        JsonObject bed = doc["bed"];
        TAKE_MUTEX(bed_settings_mutex)
        bool bed_updated = false;

        if (bed.containsKey("control_mode")) bed_settings.control_mode = static_cast<BedControlMode>(bed["control_mode"].as<uint32_t>()), bed_updated = true;
        if (bed.containsKey("screw_lead_um")) bed_settings.screw_lead_um = bed["screw_lead_um"].as<uint32_t>(), bed_updated = true;
        if (bed.containsKey("microstep_multiplier")) bed_settings.microstep_multiplier = bed["microstep_multiplier"].as<uint32_t>(), bed_updated = true;
        if (bed.containsKey("steps_per_revolution")) bed_settings.steps_per_revolution = bed["steps_per_revolution"].as<uint32_t>(), bed_updated = true;
        if (bed.containsKey("moving_speed")) bed_settings.moving_speed = bed["moving_speed"].as<uint32_t>(), bed_updated = true;
        if (bed.containsKey("homing_speed")) bed_settings.homing_speed = bed["homing_speed"].as<uint32_t>(), bed_updated = true;
        if (bed.containsKey("backoff_distance_um")) bed_settings.backoff_distance_um = bed["backoff_distance_um"].as<uint32_t>(), bed_updated = true;

        if (bed.containsKey("origin")) {
            JsonObject origin = bed["origin"];
            if (origin.containsKey("is_set")) bed_settings.origin.is_set = origin["is_set"].as<bool>();
            if (origin.containsKey("position_nm")) bed_settings.origin.position_nm = origin["position_nm"].as<int32_t>();
            bed_updated = true;
        }

        if (bed_updated) updated_settings_types |= SETTINGS_TYPE_BED;
        RELEASE_MUTEX(bed_settings_mutex)
    }

    // Probes settings
    if (doc.containsKey("probes")) {
        JsonObject probes = doc["probes"];
        TAKE_MUTEX(probes_settings_mutex)
        bool probes_updated = false;

        if (probes.containsKey("cooling")) {
            JsonObject cooling = probes["cooling"];
            if (cooling.containsKey("flow")) {
                JsonObject flow = cooling["flow"];
                if (flow.containsKey("min")) probes_settings.cooling_flow_min = flow["min"].as<float>(), probes_updated = true;
                if (flow.containsKey("max")) probes_settings.cooling_flow_max = flow["max"].as<float>(), probes_updated = true;
            }
            if (cooling.containsKey("temp")) {
                JsonObject temp = cooling["temp"];
                if (temp.containsKey("min")) probes_settings.cooling_temp_min = temp["min"].as<float>(), probes_updated = true;
                if (temp.containsKey("max")) probes_settings.cooling_temp_max = temp["max"].as<float>(), probes_updated = true;
            }
        }

        if (probes_updated) updated_settings_types |= SETTINGS_TYPE_PROBES;
        RELEASE_MUTEX(probes_settings_mutex)
    }

    // OTA settings
    if (doc.containsKey("ota")) {
        JsonObject ota = doc["ota"];
        TAKE_MUTEX(ota_settings_mutex)
        bool ota_updated = false;

        if (ota.containsKey("login")) {
            strlcpy(ota_settings.login, ota["login"].as<const char*>(), ARRAY_SIZE(ota_settings.login));
            ota_updated = true;
        }
        if (ota.containsKey("password")) {
            strlcpy(ota_settings.password, ota["password"].as<const char*>(), ARRAY_SIZE(ota_settings.password));
            ota_updated = true;
        }

        if (ota_updated) updated_settings_types |= SETTINGS_TYPE_OTA;
        RELEASE_MUTEX(ota_settings_mutex)
    }

    // GRBL settings
    if (doc.containsKey("grbl")) {
        JsonObject grbl = doc["grbl"];
        TAKE_MUTEX(grbl_settings_mutex)
        bool grbl_updated = false;

        if (grbl.containsKey("jog_speed")) grbl_settings.jog_speed = grbl["jog_speed"].as<float>(), grbl_updated = true;
        if (grbl.containsKey("default_timeout_ms")) grbl_settings.default_timeout_ms = grbl["default_timeout_ms"].as<uint32_t>(), grbl_updated = true;
        if (grbl.containsKey("homing_timeout_ms")) grbl_settings.homing_timeout_ms = grbl["homing_timeout_ms"].as<uint32_t>(), grbl_updated = true;

        if (grbl_updated) updated_settings_types |= SETTINGS_TYPE_GRBL;
        RELEASE_MUTEX(grbl_settings_mutex)
    }

    // Relays settings
    if (doc.containsKey("relays")) {
        JsonObject relays = doc["relays"];
        TAKE_MUTEX(relays_settings_mutex)
        bool relays_updated = false;

        if (relays.containsKey("alarm_behavior")) relays_settings.alarm_behavior = relays["alarm_behavior"].as<uint32_t>(), relays_updated = true;
        if (relays.containsKey("interlock_behavior")) relays_settings.interlock_behavior = relays["interlock_behavior"].as<uint32_t>(), relays_updated = true;

        if (relays_updated) updated_settings_types |= SETTINGS_TYPE_RELAYS;
        RELEASE_MUTEX(relays_settings_mutex)
    }

    // If any settings were updated, schedule a save
    if (updated_settings_types != 0) {
        settings_schedule_save(updated_settings_types);
    }
}
