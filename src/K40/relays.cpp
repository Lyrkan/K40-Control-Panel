#include <Arduino.h>

#include "Grbl/grbl_report.h"
#include "Grbl/grbl_state.h"
#include "K40/alerts.h"
#include "K40/relays.h"
#include "macros.h"
#include "mutex.h"
#include "queues.h"
#include "settings.h"

static RelayPin relay_pins[] = {RELAY_PIN_INTERLOCK, RELAY_PIN_ALARM, RELAY_PIN_LIGHTS, RELAY_PIN_ACCESSORY};

static inline uint8_t relays_get_pin_state_value(RelayPin pin, RelayState state) {
    switch (pin) {
    // Relay
    case RELAY_PIN_INTERLOCK:
        return (state == RELAY_STATE_ENABLED) ? LOW : HIGH;

    // MOSFETs
    case RELAY_PIN_LIGHTS:
    case RELAY_PIN_ACCESSORY:
    case RELAY_PIN_ALARM:
        return (state == RELAY_STATE_ENABLED) ? HIGH : LOW;
    }

    return LOW;
}

void relays_init() {
    digitalWrite(RELAY_PIN_INTERLOCK, relays_get_pin_state_value(RELAY_PIN_INTERLOCK, RELAY_STATE_ENABLED));
    digitalWrite(RELAY_PIN_ALARM, relays_get_pin_state_value(RELAY_PIN_ALARM, RELAY_STATE_ENABLED));
    digitalWrite(RELAY_PIN_LIGHTS, relays_get_pin_state_value(RELAY_PIN_LIGHTS, RELAY_STATE_ENABLED));
    digitalWrite(RELAY_PIN_ACCESSORY, relays_get_pin_state_value(RELAY_PIN_ACCESSORY, RELAY_STATE_ENABLED));
}

bool relays_is_active(RelayPin pin) { return digitalRead(pin) == relays_get_pin_state_value(pin, RELAY_STATE_ENABLED); }

bool relays_is_disabled(RelayPin pin) {
    // Only the interlock pin can currently be in disabled state
    if (pin != RELAY_PIN_INTERLOCK) {
        return false;
    }

    // Retrieve behaviors from settings
    TAKE_MUTEX(relays_settings_mutex)
    uint32_t interlock_behavior = relays_settings.interlock_behavior;
    RELEASE_MUTEX(relays_settings_mutex)

    uint8_t alerts_status = alerts_get_current_alerts();
    if ((bool)(interlock_behavior & INTERLOCK_DISABLE_WHEN_COOLING_ISSUE) && (alerts_status & (ALERT_TYPE_COOLING))) {
        return true;
    }

    if ((bool)(interlock_behavior & INTERLOCK_DISABLE_WHEN_LID_OPENED) && (alerts_status & (ALERT_TYPE_LIDS))) {
        return true;
    }

    if ((bool)(interlock_behavior & INTERLOCK_DISABLE_WHEN_FLAME_SENSOR_TRIGGERED) &&
        (alerts_status & (ALERT_TYPE_FLAME_SENSOR))) {
        return true;
    }

    return false;
}

void relays_update() {
    // Holds the indexes of force disabled relays as a bit mask
    // Those relays should be re-activated automatically when they are not disabled anymore.
    static int8_t force_disabled_indexes = 0;

    // Retrieve alarm behavior from settings
    TAKE_MUTEX(relays_settings_mutex)
    uint32_t alarm_behavior = relays_settings.alarm_behavior;
    RELEASE_MUTEX(relays_settings_mutex)

    // Check if the alarm relay should be enabled
    bool enable_alarm_relay = false;
    uint8_t alerts_status = alerts_get_current_alerts();

    TAKE_MUTEX(grbl_last_report_mutex)
    GrblState grbl_state = grbl_last_report.state;
    RELEASE_MUTEX(grbl_last_report_mutex)

    if ((bool)(alarm_behavior & ALARM_ENABLE_WHEN_COOLING_ISSUE) && (bool)(alerts_status & (ALERT_TYPE_COOLING))) {
        enable_alarm_relay = true;
    }

    if ((bool)(alarm_behavior & ALARM_ENABLE_WHEN_FLAME_SENSOR_TRIGGERED) &&
        (bool)(alerts_status & (ALERT_TYPE_FLAME_SENSOR))) {
        enable_alarm_relay = true;
    }

    if ((bool)(alarm_behavior & ALARM_ENABLE_WHEN_LID_OPENED) && (bool)(alerts_status & (ALERT_TYPE_LIDS))) {
        enable_alarm_relay = true;
    }

    if ((bool)(alarm_behavior & ALARM_ENABLE_WHEN_RUNNING) && grbl_state == GRBL_STATE_RUN) {
        enable_alarm_relay = true;
    }

    if ((bool)(alarm_behavior & ALARM_ENABLE_WHEN_NOT_IDLING) && grbl_state != GRBL_STATE_IDLE) {
        enable_alarm_relay = true;
    }

    digitalWrite(
        RELAY_PIN_ALARM,
        relays_get_pin_state_value(RELAY_PIN_ALARM, enable_alarm_relay ? RELAY_STATE_ENABLED : RELAY_STATE_DISABLED));

    // Process new command if there is one
    static RelaysCommand relays_command;
    bool current_status_updated = false;

    while (xQueueReceive(relays_command_queue, &relays_command, 0) == pdTRUE) {
        // Make sure we don't activate a relay that is supposed to be disabled
        if (relays_command.state == relays_get_pin_state_value(relays_command.pin, RELAY_STATE_ENABLED) &&
            relays_is_disabled(relays_command.pin)) {
            continue;
        }

        digitalWrite(relays_command.pin, relays_get_pin_state_value(relays_command.pin, relays_command.state));
    }

    // Make sure disabled relays are not active
    for (int i = 0; i < ARRAY_SIZE(relay_pins); i++) {
        if (relays_is_active(relay_pins[i]) && relays_is_disabled(relay_pins[i])) {
            digitalWrite(relay_pins[i], relays_get_pin_state_value(relay_pins[i], RELAY_STATE_DISABLED));
            force_disabled_indexes |= 1 << i;
        }

        // Re-activate relays that were previously force disabled if they are not in a disabled state
        if (!relays_is_disabled(relay_pins[i]) && (force_disabled_indexes & (1 << i)) != 0) {
            digitalWrite(relay_pins[i], relays_get_pin_state_value(relay_pins[i], RELAY_STATE_ENABLED));
            force_disabled_indexes &= ~(1 << i);
        }
    }
}
