#ifndef K40_RELAYS_H
#define K40_RELAYS_H

#define RELAY_PIN_STATE_ENABLED LOW
#define RELAY_PIN_STATE_DISABLED HIGH

enum RelayPin {
    PIN_RELAY_LASER = 14,
    PIN_RELAY_AIR_ASSIST = 12,
    PIN_RELAY_COOLING = 13,
    PIN_RELAY_ALARM = 4,
    PIN_RELAY_LIGHTS = 0,
    PIN_RELAY_BEAM_PREVIEW = 26
};

typedef struct RelaysStatus RelaysStatus;
struct RelaysStatus {
    bool laser_enabled;
    bool air_assist_enabled;
    bool cooling_enabled;
    bool alarm_enabled;
    bool lights_enabled;
    bool beam_preview_enabled;
};

typedef struct RelaysCommand RelaysCommand;
struct RelaysCommand {
    RelayPin pin;
    bool enable;
};

void relays_init();
void relays_update();

#endif
