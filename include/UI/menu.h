#ifndef UI_MENU_H
#define UI_MENU_H

#include <lvgl.h>

#define MENU_INDICATORS_UPDATE_INTERVAL 1000 // ms

const char MENU_INDICATORS_TIME_UNKNOWN_VALUE[] = "--:--";
const char MENU_INDICATORS_WIFI_VALUE_OFFLINE[] = "Offline";
const char MENU_INDICATORS_WIFI_VALUE_ONLINE[] = "Online";

extern lv_obj_t *ui_menu_panel;
extern lv_obj_t *ui_menu_status_button;
extern lv_obj_t *ui_menu_status_button_label;
extern lv_obj_t *ui_menu_controls_button;
extern lv_obj_t *ui_menu_controls_button_label;
extern lv_obj_t *ui_menu_bed_button;
extern lv_obj_t *ui_menu_bed_button_label;
extern lv_obj_t *ui_menu_settings_button;
extern lv_obj_t *ui_menu_settings_button_label;
extern lv_obj_t *ui_menu_indicators_panel;
extern lv_obj_t *ui_menu_indicators_current_time_value;
extern lv_obj_t *ui_menu_indicators_connectivity_value;
extern lv_obj_t *ui_menu_indicators_wifi_icon;

void ui_menu_init();
void ui_menu_indicators_update();

#endif
