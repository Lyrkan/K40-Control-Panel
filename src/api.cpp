#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "K40/cooling.h"
#include "K40/lids.h"
#include "K40/voltage_probes.h"
#include "api.h"
#include "queues.h"

void api_init(AsyncWebServer *server) {
    server->on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        DynamicJsonDocument state(1024);
        String serializedState;

        state["firmware"]["version"] = GIT_CURRENT_REF;
        state["firmware"]["build_date"] = __DATE__ " " __TIME__;

        // Retrieve sensors values
        VoltageProbesValues voltage_probes_values;
        CoolingValues cooling_values;
        LidsStates lids_states;
        bool flame_sensor_triggered;

        if (xQueuePeek(voltage_current_status_queue, &voltage_probes_values, 100.f / portTICK_RATE_MS) == pdTRUE) {
            state["sensors"]["voltages"]["v1"] = voltage_probes_values.probe1;
            state["sensors"]["voltages"]["v2"] = voltage_probes_values.probe2;
            state["sensors"]["voltages"]["v3"] = voltage_probes_values.probe3;
        } else {
            state["sensors"]["voltages"]["v1"] = nullptr;
            state["sensors"]["voltages"]["v2"] = nullptr;
            state["sensors"]["voltages"]["v3"] = nullptr;
        }

        if (xQueuePeek(cooling_current_status_queue, &cooling_values, 100.f / portTICK_RATE_MS) == pdTRUE) {
            state["sensors"]["cooling"]["flow"] = cooling_values.flow;
            state["sensors"]["cooling"]["temp"] = cooling_values.temperature;
        } else {
            state["sensors"]["cooling"]["flow"] = nullptr;
            state["sensors"]["cooling"]["temp"] = nullptr;
        }

        if (xQueuePeek(lids_current_status_queue, &lids_states, 100.f / portTICK_RATE_MS) == pdTRUE) {
            state["sensors"]["lids"]["front"] = lids_states.front_opened ? "opened" : "closed";
            state["sensors"]["lids"]["back"] = lids_states.back_opened ? "opened" : "closed";
        } else {
            state["sensors"]["lids"]["front"] = nullptr;
            state["sensors"]["lids"]["back"] = nullptr;
        }

        if (xQueuePeek(flame_sensor_current_status_queue, &flame_sensor_triggered, 0) == pdTRUE) {
            state["sensors"]["flame_sensor"]["triggered"] = flame_sensor_triggered;
        } else {
            state["sensors"]["flame_sensor"]["triggered"] = nullptr;
        }

        // TODO Retrieve alerts
        state["alerts"]["voltages"] = true;
        state["alerts"]["cooling"] = true;
        state["alerts"]["lids"] = true;
        state["alerts"]["flame_sensor"] = true;

        // TODO Retrieve relays state
        state["relays"]["laser"] = false;
        state["relays"]["air_assist"] = false;
        state["relays"]["cooling"] = false;
        state["relays"]["alarm"] = false;
        state["relays"]["lights"] = false;
        state["relays"]["beam_preview"] = false;

        serializeJsonPretty(state, serializedState);

        request->send(200, "application/json", serializedState);
    });
}
