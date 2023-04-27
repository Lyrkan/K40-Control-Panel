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

        // TODO Retrieve real values
        state["sensors"]["voltages"]["v1"] = 0.0;
        state["sensors"]["voltages"]["v2"] = 0.0;
        state["sensors"]["voltages"]["v3"] = 0.0;
        state["sensors"]["cooling"]["flow"] = 0.0;
        state["sensors"]["cooling"]["temp"] = 0.0;
        state["sensors"]["lids"]["front"] = "opened";
        state["sensors"]["lids"]["back"] = "opened";
        state["sensors"]["flame_sensor"]["triggered"] = false;
        state["alerts"]["voltages"] = true;
        state["alerts"]["cooling"] = true;
        state["alerts"]["lids"] = true;
        state["alerts"]["flame_sensor"] = true;
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
