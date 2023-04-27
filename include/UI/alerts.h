#ifndef UI_ALERTS_H
#define UI_ALERTS_H

#include <lvgl.h>

extern lv_obj_t *ui_status_laser_enabled_panel;
extern lv_obj_t *ui_status_laser_enabled_label;

void ui_alerts_init();
void ui_alerts_update();

#endif
