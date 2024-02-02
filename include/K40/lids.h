#ifndef K40_LIDS_H
#define K40_LIDS_H

typedef enum {
    PIN_LID_STATUS_FRONT = 35,
    PIN_LID_STATUS_BACK = 32
} LidPin;

typedef struct {
    bool front_opened;
    bool back_opened;
} LidsStates;

extern LidsStates lids_states;

void lids_update_status();

#endif
