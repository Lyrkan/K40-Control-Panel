#include <string.h>

#include "Grbl/grbl_state.h"

GrblState grbl_state_from_string(char *state) {
    if (strcmp(state, "Idle") == 0) {
        return GRBL_STATE_IDLE;
    }

    if (strcmp(state, "Run") == 0) {
        return GRBL_STATE_RUN;
    }

    if (strcmp(state, "Alarm") == 0) {
        return GRBL_STATE_ALARM;
    }

    if (strcmp(state, "Check") == 0) {
        return GRBL_STATE_CHECK;
    }

    if (strcmp(state, "Home") == 0) {
        return GRBL_STATE_HOME;
    }

    if (strcmp(state, "Sleep") == 0) {
        return GRBL_STATE_SLEEP;
    }

    if ((strncmp(state, "Hold:", 5) == 0) && (state[5] != '\0')) {
        // - Hold:0 - Hold complete. Ready to resume.
        // - Hold:1 - Hold in-progress. Reset will throw an alarm.
        int hold_state = atoi(state + 5);
        switch (hold_state) {
        case 0:
            return GRBL_STATE_HOLD_0;
        case 1:
            return GRBL_STATE_HOLD_1;
        }
    }

    if ((strncmp(state, "Door:", 5) == 0) && (state[5] != '\0')) {
        // - Door:0 - Door closed. Ready to resume.
        // - Door:1 - Machine stopped. Door still ajar. Can't resume until closed.
        // - Door:2 - Door opened. Hold (or parking retract) in-progress. Reset will throw an alarm.
        // - Door:3 - Door closed and resuming. Restoring from park, if applicable. Reset will throw an alarm.
        int door_state = atoi(state + 5);
        switch (door_state) {
        case 0:
            return GRBL_STATE_DOOR_0;
        case 1:
            return GRBL_STATE_DOOR_1;
        case 2:
            return GRBL_STATE_DOOR_2;
        case 3:
            return GRBL_STATE_DOOR_3;
        }
    }

    return GRBL_STATE_INVALID;
}

const char *grbl_state_to_string(GrblState state) {
    switch (state) {
    case GRBL_STATE_IDLE:
        return "Idle";
    case GRBL_STATE_RUN:
        return "Run";
    case GRBL_STATE_HOLD_0:
        return "Hold: complete";
    case GRBL_STATE_HOLD_1:
        return "Hold: in-progress";
    case GRBL_STATE_ALARM:
        return "Alarm";
    case GRBL_STATE_DOOR_0:
        return "Door: closed";
    case GRBL_STATE_DOOR_1:
        return "Door: machine stopped";
    case GRBL_STATE_DOOR_2:
        return "Door: hold";
    case GRBL_STATE_DOOR_3:
        return "Door: resuming";
    case GRBL_STATE_CHECK:
        return "Check";
    case GRBL_STATE_HOME:
        return "Home";
    case GRBL_STATE_SLEEP:
        return "Sleep";
    default:
        return "Invalid/unknown";
    }
}
