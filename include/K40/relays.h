#ifndef K40_RELAYS_H
#define K40_RELAYS_H

typedef enum {
    RELAY_STATE_ENABLED,
    RELAY_STATE_DISABLED
} RelayState;

typedef enum {
    RELAY_PIN_INTERLOCK = 12,
    RELAY_PIN_AIR_ASSIST = 14,
    RELAY_PIN_ALARM = 4,
    RELAY_PIN_LIGHTS = 13,
    RELAY_PIN_BEAM_PREVIEW = 26
} RelayPin;

RelayPin relay_pins[] = {
    RELAY_PIN_INTERLOCK, RELAY_PIN_AIR_ASSIST, RELAY_PIN_ALARM, RELAY_PIN_LIGHTS, RELAY_PIN_BEAM_PREVIEW};

typedef struct {
    RelayPin pin;
    RelayState state;
} RelaysCommand;

void relays_init();
bool relays_is_active(RelayPin pin);
bool relays_is_disabled(RelayPin pin);
void relays_update();

#endif
