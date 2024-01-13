#ifndef K40_BED_H
#define K40_BED_H

#include <math.h>

#define BED_STEP_TIMER_ID 0
#define BED_IDLE_UPDATE_INTERVAL 500
#define BED_RUNNING_UPDATE_INTERVAL 100
#define BED_LIMIT_PIN_ACTIVATION_STATE HIGH

typedef enum {
    BED_DIR_DOWN = -1,
    BED_DIR_UNKNOWN = 0,
    BED_DIR_UP = 1
} BedDirection;

typedef enum {
    PIN_BED_STEP = 16,
    PIN_BED_DIR = 17,
    PIN_BED_LIMIT = 27
} BedStepperPin;

typedef enum {
    BED_STATE_IDLE,
    BED_STATE_GOING_UP,
    BED_STATE_GOING_DOWN,
    BED_STATE_HOMING,
} BedState;

typedef enum {
    BED_COMMAND_MOVE_ABSOLUTE,
    BED_COMMAND_MOVE_RELATIVE,
    BED_COMMAND_HOME,
    BED_COMMAND_STOP,
    BED_COMMAND_SET_CURRENT_POSITION_AS_ORIGIN,
} BedCommandType;

typedef struct {
    BedCommandType type;
    int32_t value_nm;
} BedCommand;

typedef struct {
    bool is_set;
    int32_t position_nm;
} BedPosition;

typedef struct {
    BedState state;
    BedPosition target;
    BedPosition current;
    BedPosition origin;
} BedStatus;

void bed_init();
BedState bed_update();

#endif
