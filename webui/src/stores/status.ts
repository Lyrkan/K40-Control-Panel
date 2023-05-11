import { atom } from "nanostores";

export type SensorsStatus = {
    voltages: {
        v1: number | null;
        v2: number | null;
        v3: number | null;
    };
    cooling: {
        flow: number | null;
        temp: number | null;
    };
    lids: {
        front: "opened" | "closed" | null;
        back: "opened" | "closed" | null;
    };
    flame_sensor: {
        triggered: boolean | null;
    };
};

export type AlertsStatus = {
    voltage: boolean;
    cooling: boolean;
    lids: boolean;
    flame_sensor: boolean;
};

export type RelaysStatus = {
    laser: boolean;
    air_assist: boolean;
    cooling: boolean;
    alarm: boolean;
    lights: boolean;
    beam_preview: boolean;
};

export type Status = {
    sensors: SensorsStatus;
    alerts: AlertsStatus;
    relays: RelaysStatus;
};

export const statusStore = atom<Status>({
    sensors: {
        voltages: {
            v1: null,
            v2: null,
            v3: null,
        },
        cooling: {
            flow: null,
            temp: null,
        },
        lids: {
            front: null,
            back: null,
        },
        flame_sensor: {
            triggered: null,
        },
    },
    alerts: {
        voltage: false,
        cooling: false,
        lids: false,
        flame_sensor: false,
    },
    relays: {
        laser: false,
        air_assist: false,
        cooling: false,
        alarm: false,
        lights: false,
        beam_preview: false,
    },
});

export function updatesSensorsStatus(sensorsStatus: SensorsStatus) {
    statusStore.set({
        ...statusStore.get(),
        sensors: sensorsStatus,
    });
}

export function updatesAlertsStatus(alertsStatus: AlertsStatus) {
    statusStore.set({
        ...statusStore.get(),
        alerts: alertsStatus,
    });
}

export function updatesRelaysStatus(relaysStatus: RelaysStatus) {
    statusStore.set({
        ...statusStore.get(),
        relays: relaysStatus,
    });
}
