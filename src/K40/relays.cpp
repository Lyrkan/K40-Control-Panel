#include <Arduino.h>

#include "K40/alerts.h"
#include "K40/relays.h"
#include "queues.h"

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
    digitalWrite(RELAY_PIN_INTERLOCK, relays_get_pin_state_value(RELAY_PIN_INTERLOCK, RELAY_STATE_DISABLED));
    digitalWrite(RELAY_PIN_AIR_ASSIST, relays_get_pin_state_value(RELAY_PIN_AIR_ASSIST, RELAY_STATE_ENABLED));
    digitalWrite(RELAY_PIN_ALARM, relays_get_pin_state_value(RELAY_PIN_ALARM, RELAY_STATE_DISABLED));
    digitalWrite(RELAY_PIN_LIGHTS, relays_get_pin_state_value(RELAY_PIN_LIGHTS, RELAY_STATE_ENABLED));
    digitalWrite(RELAY_PIN_BEAM_PREVIEW, relays_get_pin_state_value(RELAY_PIN_BEAM_PREVIEW, RELAY_STATE_ENABLED));
}

bool relays_is_enabled(RelayPin pin) {
    return digitalRead(pin) == relays_get_pin_state_value(pin, RELAY_STATE_ENABLED);
}

void relays_update() {
    // Process new command if there is one
    static RelaysCommand relays_command;
    bool current_status_updated = false;

    while (xQueueReceive(relays_command_queue, &relays_command, 0) == pdTRUE) {
        Serial.print(relays_command.pin);
        Serial.print("=");
        Serial.print(relays_command.state);
        Serial.print(" (");
        Serial.print(relays_get_pin_state_value(relays_command.pin, relays_command.state));
        Serial.println(")");
        digitalWrite(relays_command.pin, relays_get_pin_state_value(relays_command.pin, relays_command.state));
    }

    if (relays_is_enabled(RELAY_PIN_INTERLOCK)) {
        uint8_t alerts_status = alerts_get_current_alerts();
        bool disable_laser = (alerts_status & (ALERT_TYPE_COOLING | ALERT_TYPE_LIDS)) != 0;

        // Disable laser if cooling is not enabled or if there is a critical alert (cooling or lid)
        if (disable_laser) {
            digitalWrite(RELAY_PIN_INTERLOCK, relays_get_pin_state_value(RELAY_PIN_INTERLOCK, RELAY_STATE_DISABLED));
        }
    }
}
