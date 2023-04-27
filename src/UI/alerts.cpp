#include <Arduino.h>
#include <lvgl.h>

#include "UI/images.h"
#include "UI/alerts.h"
#include "UI/screens/controls.h"
#include "queues.h"

lv_obj_t *ui_status_laser_enabled_label;

void ui_alerts_init() {
    ui_status_laser_enabled_label = lv_label_create(lv_layer_top());
    lv_obj_set_width(ui_status_laser_enabled_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_laser_enabled_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_status_laser_enabled_label, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_x(ui_status_laser_enabled_label, -25);
    lv_obj_set_y(ui_status_laser_enabled_label, -20);
    lv_label_set_recolor(ui_status_laser_enabled_label, true);
    lv_label_set_text(ui_status_laser_enabled_label, "#FF0000 " LV_SYMBOL_WARNING "# Laser enabled");
    lv_obj_set_style_text_color(ui_status_laser_enabled_label, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_laser_enabled_label, &font_default_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_status_laser_enabled_label, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_status_laser_enabled_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_status_laser_enabled_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_status_laser_enabled_label, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_status_laser_enabled_label, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_status_laser_enabled_label, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_status_laser_enabled_label, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_status_laser_enabled_label, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_status_laser_enabled_label, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_status_laser_enabled_label, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_status_laser_enabled_label, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_status_laser_enabled_label, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_status_laser_enabled_label, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui_status_laser_enabled_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui_status_laser_enabled_label, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void ui_alerts_update() {
    if (lv_obj_has_state(ui_controls_laser_switch, LV_STATE_CHECKED)) {
        if (lv_obj_has_flag(ui_status_laser_enabled_label, LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_clear_flag(ui_status_laser_enabled_label, LV_OBJ_FLAG_HIDDEN);
        }
    } else {
        if (!lv_obj_has_flag(ui_status_laser_enabled_label, LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_add_flag(ui_status_laser_enabled_label, LV_OBJ_FLAG_HIDDEN);
        }
    }
}
