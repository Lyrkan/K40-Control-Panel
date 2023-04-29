#include <Arduino.h>

#include "K40/relays.h"
#include "queues.h"

static RelaysStatus relays_current_status = {
    .laser_enabled = false,
    .air_assist_enabled = true,
    .cooling_enabled = true,
    .alarm_enabled = false,
    .lights_enabled = true,
    .beam_preview_enabled = true,
};

static void relays_update_pins() {
    digitalWrite(
        PIN_RELAY_LASER,
        relays_current_status.laser_enabled ? RELAY_PIN_STATE_ENABLED : RELAY_PIN_STATE_DISABLED);
    digitalWrite(
        PIN_RELAY_AIR_ASSIST,
        relays_current_status.air_assist_enabled ? RELAY_PIN_STATE_ENABLED : RELAY_PIN_STATE_DISABLED);
    digitalWrite(
        PIN_RELAY_COOLING,
        relays_current_status.cooling_enabled ? RELAY_PIN_STATE_ENABLED : RELAY_PIN_STATE_DISABLED);
    digitalWrite(
        PIN_RELAY_ALARM,
        relays_current_status.alarm_enabled ? RELAY_PIN_STATE_ENABLED : RELAY_PIN_STATE_DISABLED);
    digitalWrite(
        PIN_RELAY_LIGHTS,
        relays_current_status.lights_enabled ? RELAY_PIN_STATE_ENABLED : RELAY_PIN_STATE_DISABLED);
    digitalWrite(
        PIN_RELAY_BEAM_PREVIEW,
        relays_current_status.beam_preview_enabled ? RELAY_PIN_STATE_ENABLED : RELAY_PIN_STATE_DISABLED);
}

void relays_init() {
    relays_update_pins();
    xQueueOverwrite(relays_current_status_queue, &relays_current_status);
}

void relays_update() {
    // Process new command if there is one
    static RelaysCommand relays_command;
    bool current_status_updated = false;

    while (xQueueReceive(relays_command_queue, &relays_command, 0) == pdTRUE) {
        current_status_updated = true;
        switch (relays_command.pin) {
        case PIN_RELAY_LASER:
            relays_current_status.laser_enabled = relays_command.enable;
            break;
        case PIN_RELAY_AIR_ASSIST:
            relays_current_status.air_assist_enabled = relays_command.enable;
            break;
        case PIN_RELAY_COOLING:
            relays_current_status.cooling_enabled = relays_command.enable;
            break;
        case PIN_RELAY_ALARM:
            relays_current_status.alarm_enabled = relays_command.enable;
            break;
        case PIN_RELAY_LIGHTS:
            relays_current_status.lights_enabled = relays_command.enable;
            break;
        case PIN_RELAY_BEAM_PREVIEW:
            relays_current_status.beam_preview_enabled = relays_command.enable;
            break;
        }
    }

    if (relays_current_status.laser_enabled) {
        // Disable laser if cooling is not enabled
        if (!relays_current_status.cooling_enabled) {
            relays_current_status.laser_enabled = false;
            current_status_updated = true; // Shouldn't be needed but just to be safe...
        }

        // TODO Update laser pin status based on current alerts
    }

    // Update pins statuses and current status queue if needed
    if (current_status_updated) {
        relays_update_pins();
        xQueueOverwrite(relays_current_status_queue, &relays_current_status);
    }
}
