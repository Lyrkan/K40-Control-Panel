#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <math.h>

#include "K40/alerts.h"
#include "K40/cooling.h"
#include "K40/lids.h"
#include "K40/voltage_probes.h"
#include "K40/relays.h"
#include "api.h"
#include "cpu_monitor.h"
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

        // Retrieve alerts
        uint8_t alerts_status = alerts_get_current_alerts();
        state["alerts"]["voltages"] = (alerts_status & ALERT_TYPE_VOLTAGE) != 0;
        state["alerts"]["cooling"] = (alerts_status & ALERT_TYPE_COOLING) != 0;
        state["alerts"]["lids"] = (alerts_status & ALERT_TYPE_LIDS) != 0;
        state["alerts"]["flame_sensor"] = (alerts_status & ALERT_TYPE_FLAME_SENSOR) != 0;

        // Retrieve relays state from the queue object
        RelaysStatus current_relays_status;
        if (xQueuePeek(relays_current_status_queue, &current_relays_status, 0) == pdTRUE) {
            state["relays"]["laser"] = current_relays_status.laser_enabled;
            state["relays"]["air_assist"] = current_relays_status.air_assist_enabled;
            state["relays"]["cooling"] = current_relays_status.cooling_enabled;
            state["relays"]["alarm"] = current_relays_status.alarm_enabled;
            state["relays"]["lights"] = current_relays_status.lights_enabled;
            state["relays"]["beam_preview"] = current_relays_status.beam_preview_enabled;
        } else {
            state["relays"]["laser"] = nullptr;
            state["relays"]["air_assist"] = nullptr;
            state["relays"]["cooling"] = nullptr;
            state["relays"]["alarm"] = nullptr;
            state["relays"]["lights"] = nullptr;
            state["relays"]["beam_preview"] = nullptr;
        }

        // Retrieve system data
        xSemaphoreTake(cpu_monitor_stats_mutex, portMAX_DELAY);
        float_t core_usage_percentage_0 = cpu_monitor_load_0;
        float_t core_usage_percentage_1 = cpu_monitor_load_1;
        xSemaphoreGive(cpu_monitor_stats_mutex);

        state["system"]["chip"]["model"] = ESP.getChipModel();
        state["system"]["chip"]["revision"] = ESP.getChipRevision();
        state["system"]["heap"]["free"] = ESP.getFreeHeap();
        state["system"]["heap"]["total"] = ESP.getHeapSize();
        state["system"]["cpu"]["freq_mhz"] = ESP.getCpuFreqMHz();
        state["system"]["cpu"]["load_percent"]["core_0"] = core_usage_percentage_0;
        state["system"]["cpu"]["load_percent"]["core_1"] = core_usage_percentage_1;

        // Serialize JSON data and send it to the client
        serializeJsonPretty(state, serializedState);
        request->send(200, "application/json", serializedState);
    });
}
