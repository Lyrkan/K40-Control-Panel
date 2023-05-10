#ifndef UI_SCREEN_BED_H
#define UI_SCREEN_BED_H

#include <lvgl.h>

#define BED_STATE_UPDATE_INTERVAL 100 // ms

extern lv_obj_t *ui_bed_screen;

void ui_bed_init();
void ui_bed_update(bool initialize = false);

#endif
