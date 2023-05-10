#ifndef UI_SCREEN_CONTROLS_H
#define UI_SCREEN_CONTROLS_H

#include <lvgl.h>

#define CONTROLS_STATE_UPDATE_INTERVAL 500 // ms

extern lv_obj_t *ui_controls_screen;

void ui_controls_init();
void ui_controls_update();

#endif
