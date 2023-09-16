#ifndef K40_BED_H
#define K40_BED_H

#include <math.h>

#define BED_STEP_TIMER_ID 0
#define BED_IDLE_UPDATE_INTERVAL 500
#define BED_RUNNING_UPDATE_INTERVAL 100

enum BedDirection {
    BED_DIR_DOWN = -1,
    BED_DIR_UNKNOWN = 0,
    BED_DIR_UP = 1
};

enum BedStepperPin {
    PIN_BED_STEP = 16,
    PIN_BED_DIR = 17,
    PIN_BED_LIMIT = 27
};

enum BedState {
    BED_STATE_IDLE,
    BED_STATE_GOING_UP,
    BED_STATE_GOING_DOWN,
    BED_STATE_HOMING,
};

enum BedCommandType {
    BED_COMMAND_MOVE_ABSOLUTE,
    BED_COMMAND_MOVE_RELATIVE,
    BED_COMMAND_HOME,
    BED_COMMAND_STOP,
    BED_COMMAND_SET_CURRENT_POSITION_AS_ORIGIN,
};

typedef struct BedCommand BedCommand;
struct BedCommand {
    BedCommandType type;
    int32_t value_nm;
};

typedef struct BedPosition BedPosition;
struct BedPosition {
    bool is_set;
    int32_t position_nm;
};

typedef struct BedStatus BedStatus;
struct BedStatus {
    BedState state;
    BedPosition target;
    BedPosition current;
    BedPosition origin;
};

void bed_init();
BedState bed_update();

#endif
