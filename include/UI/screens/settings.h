#ifndef UI_SCREEN_SETTINGS_H
#define UI_SCREEN_SETTINGS_H

#include <lvgl.h>

#define SETTINGS_STATE_UPDATE_INTERVAL 500 // ms

extern lv_obj_t *ui_settings_screen;

void ui_settings_init();
void ui_settings_update(bool initialize = false);

#endif
