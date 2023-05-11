import {
    AlertsStatus,
    RelaysStatus,
    SensorsStatus,
    updatesAlertsStatus,
    updatesRelaysStatus,
    updatesSensorsStatus,
} from "../stores/status.js";

class StatusService {
    async updateStatus() {
        const response = await fetch(
            `${window.location.protocol}//${window.location.host}/api/status`
        );
        const decodedResponse = await response.json();

        if (decodedResponse.sensors) {
            updatesSensorsStatus(decodedResponse.sensors as SensorsStatus);
        }

        if (decodedResponse.alerts) {
            updatesAlertsStatus(decodedResponse.alerts as AlertsStatus);
        }

        if (decodedResponse.relays) {
            updatesRelaysStatus(decodedResponse.relays as RelaysStatus);
        }

        return decodedResponse;
    }
}

export default new StatusService();
