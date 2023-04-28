#ifndef UI_SCREEN_STATUS_H
#define UI_SCREEN_STATUS_H

#include <lvgl.h>

#define STATUS_HEAP_UPDATE_INTERVAL 500

extern lv_obj_t *ui_status_screen;

void ui_status_init();
void ui_status_update();

#endif
