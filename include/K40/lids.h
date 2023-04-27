#ifndef K40_LIDS_H
#define K40_LIDS_H

enum LidPin {
    PIN_LID_STATUS_FRONT = 35,
    PIN_LID_STATUS_BACK = 32
};

typedef struct LidsStates LidsStates;
struct LidsStates {
    bool front_opened;
    bool back_opened;
};

void lids_update_status();

#endif
