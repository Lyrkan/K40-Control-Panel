#include <Arduino.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <ArduinoJson.h>

#include "Headless/headless_parser.h"
#include "Headless/headless_serial.h"
#include "K40/relays.h"
#include "Grbl/grbl_serial.h"
#include "queues.h"
#include "settings.h"

void headless_process_line(char *line) {
    // Make sure we have at least one char
    if (line[0] == '\0') {
        log_d("Ignoring the current buffer since it was empty (probably caused by a CRLF being sent)");
        return;
    }

    log_d("Received Headless data: %s", line);

    // Parse JSON input
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, line);
    if (error) {
        log_e("Failed to parse JSON input: %s", error.c_str());
        return;
    }

    // Validate required fields
    if (!doc.containsKey("a")) {
        log_e("Missing required field 'a' (action)");
        return;
    }

    HeadlessActionType action = static_cast<HeadlessActionType>(doc["a"].as<int>());
    JsonVariant payload = doc["p"];

    switch (action) {
    case HEADLESS_ACTION_TYPE_GRBL: {
        if (!payload.containsKey("message") || !payload.containsKey("id")) {
            log_e("GRBL action requires 'message' and 'id' fields in payload");
            return;
        }

        const char *message = payload["message"];
        static int message_id;
        message_id = payload["id"];

        // Setup callbacks to send acknowledgment
        GrblCommandCallbacks callbacks;
        callbacks.on_success = []() -> void {
            StaticJsonDocument<128> ack_payload;
            ack_payload["id"] = message_id;
            ack_payload["success"] = true;
            headless_send_message(HEADLESS_MESSAGE_TYPE_GRBL_ACK, ack_payload);
        };
        callbacks.on_failure = []() -> void {
            StaticJsonDocument<128> ack_payload;
            ack_payload["id"] = message_id;
            ack_payload["success"] = false;
            headless_send_message(HEADLESS_MESSAGE_TYPE_GRBL_ACK, ack_payload);
        };

        grbl_send_message(message, callbacks);
        break;
    }

    case HEADLESS_ACTION_TYPE_SETTINGS_GET:
    case HEADLESS_ACTION_TYPE_SETTINGS_SET: {
        if (action == HEADLESS_ACTION_TYPE_SETTINGS_SET) {
            if (!payload.is<JsonObject>()) {
                log_e("Settings SET action requires an object payload");
            } else {
                JsonObject settings = payload.as<JsonObject>();
                uint32_t updated_settings_types = 0;
                settings_update_from_json(settings);
            }
        }

        // Send back the updated settings
        StaticJsonDocument<512> settings_payload;
        settings_get_json(settings_payload);
        headless_send_message(HEADLESS_MESSAGE_TYPE_SETTINGS, settings_payload);
        break;
    }

    case HEADLESS_ACTION_TYPE_STATUS:
        headless_notify_update(HEADLESS_UPDATE_STATUS);
        break;

    case HEADLESS_ACTION_TYPE_RELAYS_SET: {
        if (!payload.is<JsonObject>()) {
            log_e("Relays SET action requires an object payload");
            return;
        }

        JsonObject relays = payload.as<JsonObject>();

        // Handle interlock relay
        if (relays.containsKey("interlock")) {
            RelaysCommand command = {
                .pin = RELAY_PIN_INTERLOCK,
                .state = relays["interlock"].as<bool>() ? RELAY_STATE_ENABLED : RELAY_STATE_DISABLED,
            };
            xQueueSendToBack(relays_command_queue, &command, pdMS_TO_TICKS(100));
        }

        // Handle air assist
        if (relays.containsKey("air_assist")) {
            GrblCommandCallbacks callbacks;
            callbacks.on_failure = []() -> void { log_e("Air assist toggle failed or timed out"); };
            grbl_toogle_air_assist(relays["air_assist"].as<bool>(), callbacks);
        }

        // Handle lights
        if (relays.containsKey("lights")) {
            RelaysCommand command = {
                .pin = RELAY_PIN_LIGHTS,
                .state = relays["lights"].as<bool>() ? RELAY_STATE_ENABLED : RELAY_STATE_DISABLED,
            };
            xQueueSendToBack(relays_command_queue, &command, pdMS_TO_TICKS(100));
        }

        // Handle accessory
        if (relays.containsKey("accessory")) {
            RelaysCommand command = {
                .pin = RELAY_PIN_ACCESSORY,
                .state = relays["accessory"].as<bool>() ? RELAY_STATE_ENABLED : RELAY_STATE_DISABLED,
            };
            xQueueSendToBack(relays_command_queue, &command, pdMS_TO_TICKS(100));
        }
        break;
    }

    default:
        log_e("Unknown action type: %d", action);
        break;
    }
}
