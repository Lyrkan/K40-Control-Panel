#ifndef UI_SCREEN_CONTROLS_H
#define UI_SCREEN_CONTROLS_H

#include <lvgl.h>

#define CONTROLS_STATE_UPDATE_INTERVAL 500 // ms

extern lv_obj_t *ui_controls_screen;
extern lv_obj_t *ui_controls_laser_switch;
extern lv_obj_t *ui_controls_air_assist_switch;
extern lv_obj_t *ui_controls_cooling_switch;
extern lv_obj_t *ui_controls_lights_switch;
extern lv_obj_t *ui_controls_preview_switch;

void ui_controls_init();
void ui_controls_update();

#endif
