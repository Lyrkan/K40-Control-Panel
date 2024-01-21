#ifndef GRBL_STATE_H
#define GRBL_STATE_H

typedef enum {
    GRBL_STATE_INVALID,
    GRBL_STATE_IDLE,
    GRBL_STATE_RUN,
    GRBL_STATE_HOLD_0,
    GRBL_STATE_HOLD_1,
    GRBL_STATE_ALARM,
    GRBL_STATE_DOOR_0,
    GRBL_STATE_DOOR_1,
    GRBL_STATE_DOOR_2,
    GRBL_STATE_DOOR_3,
    GRBL_STATE_CHECK,
    GRBL_STATE_HOME,
    GRBL_STATE_SLEEP,
} GrblState;

GrblState grbl_state_from_string(char *state);
const char *grbl_state_to_string(GrblState state);

#endif
