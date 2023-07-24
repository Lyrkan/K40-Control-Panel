import { MockMethod } from "vite-plugin-mock";
export default [
    {
        url: "/api/info",
        method: "get",
        statusCode: 200,
        response: () => ({
            firmware: {
                version: "9da28a82",
                build_date: "Apr 30 2023 18:31:43",
            },
            system: {
                chip: {
                    model: "ESP32-D0WDQ5",
                    revision: 1,
                },
                heap: {
                    free: 80268,
                    total: 257148,
                },
                cpu: {
                    freq_mhz: 240,
                    load_percent: {
                        core_0: 0,
                        core_1: 0.100000001,
                    },
                },
            },
        }),
    },
    {
        url: "/api/sensors",
        method: "get",
        statusCode: 200,
        response: () => ({
            sensors: {
                voltages: {
                    v1: 4.900000095,
                    v2: 12.10000038,
                    v3: 18,
                },
                cooling: {
                    flow: 5.619999886,
                    temp: 18.89999962,
                },
                lids: {
                    front: "opened",
                    back: "closed",
                },
                flame_sensor: {
                    triggered: false,
                },
            },
        }),
    },
    {
        url: "/api/alerts",
        method: "get",
        statusCode: 200,
        response: () => ({
            alerts: {
                voltages: true,
                cooling: false,
                lids: true,
                flame_sensor: false,
            },
        }),
    },
    {
        url: "/api/relays",
        method: "get",
        statusCode: 200,
        response: () => ({
            relays: {
                laser: false,
                air_assist: true,
                cooling: true,
                alarm: false,
                lights: true,
                beam_preview: true,
            },
        }),
    },
] as MockMethod[];
