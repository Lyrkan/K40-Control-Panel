import {
    AlertsStatus,
    RelaysStatus,
    SensorsStatus,
    updatesAlertsStatus,
    updatesRelaysStatus,
    updatesSensorsStatus,
} from "../stores/status.js";

class StatusService {
    async updateSensors() {
        const response = await fetch(
            `${window.location.protocol}//${window.location.host}/api/sensors`
        );
        const decodedResponse = await response.json();

        if (decodedResponse.sensors) {
            updatesSensorsStatus(decodedResponse.sensors as SensorsStatus);
        }

        return decodedResponse;
    }

    async updateAlerts() {
        const response = await fetch(
            `${window.location.protocol}//${window.location.host}/api/alerts`
        );
        const decodedResponse = await response.json();

        if (decodedResponse.alerts) {
            updatesAlertsStatus(decodedResponse.alerts as AlertsStatus);
        }

        return decodedResponse;
    }

    async updateRelays() {
        const response = await fetch(
            `${window.location.protocol}//${window.location.host}/api/relays`
        );
        const decodedResponse = await response.json();

        if (decodedResponse.relays) {
            updatesRelaysStatus(decodedResponse.relays as RelaysStatus);
        }

        return decodedResponse;
    }
}

export default new StatusService();
