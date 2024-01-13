#include <Arduino.h>

#include "K40/alerts.h"
#include "K40/relays.h"
#include "macros.h"
#include "queues.h"

static RelayPin relay_pins[] = {
    RELAY_PIN_INTERLOCK, RELAY_PIN_AIR_ASSIST, RELAY_PIN_ALARM, RELAY_PIN_LIGHTS, RELAY_PIN_BEAM_PREVIEW};

static inline uint8_t relays_get_pin_state_value(RelayPin pin, RelayState state) {
    switch (pin) {
    // Relays board
    case RELAY_PIN_INTERLOCK:
    case RELAY_PIN_AIR_ASSIST:
    case RELAY_PIN_ALARM:
        return (state == RELAY_STATE_ENABLED) ? LOW : HIGH;

    // MOSFETs
    case RELAY_PIN_LIGHTS:
    case RELAY_PIN_BEAM_PREVIEW:
        return (state == RELAY_STATE_ENABLED) ? HIGH : LOW;
    }

    return LOW;
}

void relays_init() {
    digitalWrite(RELAY_PIN_INTERLOCK, relays_get_pin_state_value(RELAY_PIN_INTERLOCK, RELAY_STATE_ENABLED));
    digitalWrite(RELAY_PIN_AIR_ASSIST, relays_get_pin_state_value(RELAY_PIN_AIR_ASSIST, RELAY_STATE_ENABLED));
    digitalWrite(RELAY_PIN_ALARM, relays_get_pin_state_value(RELAY_PIN_ALARM, RELAY_STATE_DISABLED));
    digitalWrite(RELAY_PIN_LIGHTS, relays_get_pin_state_value(RELAY_PIN_LIGHTS, RELAY_STATE_ENABLED));
    digitalWrite(RELAY_PIN_BEAM_PREVIEW, relays_get_pin_state_value(RELAY_PIN_BEAM_PREVIEW, RELAY_STATE_ENABLED));
}

bool relays_is_active(RelayPin pin) { return digitalRead(pin) == relays_get_pin_state_value(pin, RELAY_STATE_ENABLED); }

bool relays_is_disabled(RelayPin pin) {
    // Only the interlock pin can currently be in disabled state
    if (pin != RELAY_PIN_INTERLOCK) {
        return true;
    }

    uint8_t alerts_status = alerts_get_current_alerts();
    return (alerts_status & (ALERT_TYPE_COOLING | ALERT_TYPE_LIDS)) != 0;
}

void relays_update() {
    // Holds the indexes of force disabled relays as a bit mask
    // Those relays should be re-activated automatically when they are not disabled anymore.
    static int8_t force_disabled_indexes = 0;

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
