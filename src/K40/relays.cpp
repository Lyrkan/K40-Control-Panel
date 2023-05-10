#include <Arduino.h>

#include "K40/alerts.h"
#include "K40/relays.h"
#include "queues.h"

void relays_init() {}

void relays_update() {
    // Process new command if there is one
    static RelaysCommand relays_command;
    bool current_status_updated = false;

    while (xQueueReceive(relays_command_queue, &relays_command, 0) == pdTRUE) {
        digitalWrite(relays_command.pin, relays_command.enable ? RELAY_PIN_STATE_ENABLED : RELAY_PIN_STATE_DISABLED);
    }

    if (digitalRead(PIN_RELAY_LASER) == RELAY_PIN_STATE_ENABLED) {
        uint8_t alerts_status = alerts_get_current_alerts();
        bool disable_laser = (alerts_status & (ALERT_TYPE_COOLING | ALERT_TYPE_LIDS)) != 0;

        // Disable laser if cooling is not enabled or if there is a critical alert (cooling or lid)
        if (disable_laser) {
            digitalWrite(PIN_RELAY_LASER, RELAY_PIN_STATE_DISABLED);
        }
    }
}
