#ifndef UI_SCREEN_CONTROLS_H
#define UI_SCREEN_CONTROLS_H

#include <lvgl.h>

#define CONTROLS_STATE_UPDATE_INTERVAL 500 // ms

typedef enum {
    CONTROLS_UPDATE_GRBL_REPORT = 1 << 0,
    CONTROLS_UPDATE_GRBL_COMMMAND_ENDED = 1 << 1,
} ControlsScreenUpdateType;

extern lv_obj_t *ui_controls_screen;

void ui_controls_init();
void ui_controls_update(bool initialize = false);
void ui_controls_notify_update(uint8_t update_types);

#endif
