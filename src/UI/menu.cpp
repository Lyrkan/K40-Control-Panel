#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>

#include "UI/images.h"
#include "UI/menu.h"
#include "UI/screens/status.h"
#include "UI/screens/controls.h"
#include "UI/screens/bed.h"
#include "UI/screens/settings.h"
#include "macros.h"
#include "ntp.h"

static bool ui_menu_initialized = false;

lv_obj_t *ui_menu_panel;
lv_obj_t *ui_menu_status_button;
lv_obj_t *ui_menu_status_button_label;
lv_obj_t *ui_menu_controls_button;
lv_obj_t *ui_menu_controls_button_label;
lv_obj_t *ui_menu_bed_button;
lv_obj_t *ui_menu_bed_button_label;
lv_obj_t *ui_menu_settings_button;
lv_obj_t *ui_menu_settings_button_label;
lv_obj_t *ui_menu_indicators_panel;
lv_obj_t *ui_menu_indicators_current_time_value;
lv_obj_t *ui_menu_indicators_connectivity_value;
lv_obj_t *ui_menu_indicators_wifi_icon;

void ui_menu_button_event_handler(lv_event_t *e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (event_code != LV_EVENT_CLICKED) {
        return;
    }

    // Switch screen
    if (target == ui_menu_status_button) {
        lv_scr_load(ui_status_screen);
    } else if (target == ui_menu_controls_button) {
        lv_scr_load(ui_controls_screen);
    } else if (target == ui_menu_bed_button) {
        lv_scr_load(ui_bed_screen);
    } else if (target == ui_menu_settings_button) {
        lv_scr_load(ui_settings_screen);
    }

    // Update active button indicator
    lv_obj_set_style_border_opa(
        ui_menu_status_button,
        target == ui_menu_status_button ? 255 : 0,
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(
        ui_menu_controls_button,
        target == ui_menu_controls_button ? 255 : 0,
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(
        ui_menu_bed_button,
        target == ui_menu_bed_button ? 255 : 0,
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(
        ui_menu_settings_button,
        target == ui_menu_settings_button ? 255 : 0,
        LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(
        ui_menu_status_button_label,
        target == ui_menu_status_button ? lv_color_hex(0x000000) : lv_color_hex(0x808080),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(
        ui_menu_controls_button_label,
        target == ui_menu_controls_button ? lv_color_hex(0x000000) : lv_color_hex(0x808080),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(
        ui_menu_bed_button_label,
        target == ui_menu_bed_button ? lv_color_hex(0x000000) : lv_color_hex(0x808080),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(
        ui_menu_settings_button_label,
        target == ui_menu_settings_button ? lv_color_hex(0x000000) : lv_color_hex(0x808080),
        LV_PART_MAIN | LV_STATE_DEFAULT);
}

void ui_menu_init() {
    ui_menu_panel = lv_obj_create(lv_layer_top());
    lv_obj_set_width(ui_menu_panel, 480);
    lv_obj_set_height(ui_menu_panel, MENU_HEIGHT);
    lv_obj_set_style_radius(ui_menu_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_menu_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_menu_panel, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_menu_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_menu_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_menu_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_menu_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_menu_panel, LV_OBJ_FLAG_SCROLLABLE);

    ui_menu_status_button = lv_btn_create(ui_menu_panel);
    lv_obj_set_width(ui_menu_status_button, 100);
    lv_obj_set_height(ui_menu_status_button, MENU_HEIGHT);
    lv_obj_set_style_radius(ui_menu_status_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_menu_status_button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_menu_status_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_menu_status_button, lv_color_hex(0xC8C8F0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_menu_status_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_menu_status_button, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_menu_status_button, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_menu_status_button, ui_menu_button_event_handler, LV_EVENT_ALL, NULL);

    ui_menu_status_button_label = lv_label_create(ui_menu_status_button);
    lv_obj_set_width(ui_menu_status_button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_menu_status_button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_menu_status_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_menu_status_button_label, LV_SYMBOL_MAGNIFYING_GLASS " Status");
    lv_obj_set_style_text_color(ui_menu_status_button_label, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_menu_status_button_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_menu_controls_button = lv_btn_create(ui_menu_panel);
    lv_obj_set_width(ui_menu_controls_button, 100);
    lv_obj_set_height(ui_menu_controls_button, MENU_HEIGHT);
    lv_obj_set_x(ui_menu_controls_button, 100);
    lv_obj_set_y(ui_menu_controls_button, 0);
    lv_obj_set_style_radius(ui_menu_controls_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_menu_controls_button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_menu_controls_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_menu_controls_button, lv_color_hex(0xC8C8F0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_menu_controls_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_menu_controls_button, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_menu_controls_button, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_menu_controls_button, ui_menu_button_event_handler, LV_EVENT_ALL, NULL);

    ui_menu_controls_button_label = lv_label_create(ui_menu_controls_button);
    lv_obj_set_width(ui_menu_controls_button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_menu_controls_button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_menu_controls_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_menu_controls_button_label, LV_SYMBOL_TOGGLE_OFF " Controls");
    lv_obj_set_style_text_color(ui_menu_controls_button_label, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_menu_controls_button_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_menu_bed_button = lv_btn_create(ui_menu_panel);
    lv_obj_set_width(ui_menu_bed_button, 100);
    lv_obj_set_height(ui_menu_bed_button, MENU_HEIGHT);
    lv_obj_set_x(ui_menu_bed_button, 200);
    lv_obj_set_y(ui_menu_bed_button, 0);
    lv_obj_set_style_radius(ui_menu_bed_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_menu_bed_button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_menu_bed_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_menu_bed_button, lv_color_hex(0xC8C8F0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_menu_bed_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_menu_bed_button, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_menu_bed_button, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_menu_bed_button, ui_menu_button_event_handler, LV_EVENT_ALL, NULL);

    ui_menu_bed_button_label = lv_label_create(ui_menu_bed_button);
    lv_obj_set_width(ui_menu_bed_button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_menu_bed_button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_menu_bed_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_menu_bed_button_label, LV_SYMBOL_ARROW_DOWN_UP_ACROSS_LINE " Bed");
    lv_obj_set_style_text_color(ui_menu_bed_button_label, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_menu_bed_button_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_menu_settings_button = lv_btn_create(ui_menu_panel);
    lv_obj_set_width(ui_menu_settings_button, 100);
    lv_obj_set_height(ui_menu_settings_button, MENU_HEIGHT);
    lv_obj_set_x(ui_menu_settings_button, 300);
    lv_obj_set_y(ui_menu_settings_button, 0);
    lv_obj_set_style_radius(ui_menu_settings_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_menu_settings_button, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_menu_settings_button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_menu_settings_button, lv_color_hex(0xC8C8F0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_menu_settings_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_menu_settings_button, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_menu_settings_button, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_menu_settings_button, ui_menu_button_event_handler, LV_EVENT_ALL, NULL);

    ui_menu_settings_button_label = lv_label_create(ui_menu_settings_button);
    lv_obj_set_width(ui_menu_settings_button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_menu_settings_button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_menu_settings_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_menu_settings_button_label, LV_SYMBOl_GEAR " Settings");
    lv_obj_set_style_text_color(ui_menu_settings_button_label, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_menu_settings_button_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_menu_indicators_panel = lv_obj_create(ui_menu_panel);
    lv_obj_set_width(ui_menu_indicators_panel, 80);
    lv_obj_set_height(ui_menu_indicators_panel, MENU_HEIGHT);
    lv_obj_set_align(ui_menu_indicators_panel, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_radius(ui_menu_indicators_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_menu_indicators_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_menu_indicators_panel, LV_OBJ_FLAG_SCROLLABLE);

    ui_menu_indicators_current_time_value = lv_label_create(ui_menu_indicators_panel);
    lv_obj_set_width(ui_menu_indicators_current_time_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_menu_indicators_current_time_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_menu_indicators_current_time_value, 0);
    lv_obj_set_y(ui_menu_indicators_current_time_value, -8);
    lv_obj_set_align(ui_menu_indicators_current_time_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_menu_indicators_current_time_value, MENU_INDICATORS_TIME_UNKNOWN_VALUE);

    ui_menu_indicators_connectivity_value = lv_label_create(ui_menu_indicators_panel);
    lv_obj_set_width(ui_menu_indicators_connectivity_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_menu_indicators_connectivity_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_menu_indicators_connectivity_value, 5);
    lv_obj_set_y(ui_menu_indicators_connectivity_value, 8);
    lv_obj_set_align(ui_menu_indicators_connectivity_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_menu_indicators_connectivity_value, MENU_INDICATORS_WIFI_VALUE_OFFLINE);
    lv_obj_set_style_text_font(
        ui_menu_indicators_connectivity_value,
        &font_default_12,
        LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_menu_indicators_wifi_icon = lv_img_create(ui_menu_indicators_panel);
    lv_img_set_src(ui_menu_indicators_wifi_icon, &icon_wifi_offline);
    lv_obj_set_width(ui_menu_indicators_wifi_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_menu_indicators_wifi_icon, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_menu_indicators_wifi_icon, -28);
    lv_obj_set_y(ui_menu_indicators_wifi_icon, 8);
    lv_obj_set_align(ui_menu_indicators_wifi_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_menu_indicators_wifi_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_menu_indicators_wifi_icon, LV_OBJ_FLAG_SCROLLABLE);

    ui_menu_initialized = true;
}

void ui_menu_indicators_update() {
    static unsigned long last_indicators_update = 0;
    unsigned long current_time = millis();

    if (!ui_menu_initialized) {
        return;
    }

    if (current_time - last_indicators_update > MENU_INDICATORS_UPDATE_INTERVAL) {
        // Update WiFi status
        if (WiFi.isConnected()) {
            lv_img_set_src(ui_menu_indicators_wifi_icon, &icon_wifi_online);
            lv_label_set_text(ui_menu_indicators_connectivity_value, MENU_INDICATORS_WIFI_VALUE_ONLINE);
        } else {
            lv_img_set_src(ui_menu_indicators_wifi_icon, &icon_wifi_offline);
            lv_label_set_text(ui_menu_indicators_connectivity_value, MENU_INDICATORS_WIFI_VALUE_OFFLINE);
        }

        // Update current time
        static char current_formatted_time[MAX(ARRAY_SIZE(MENU_INDICATORS_TIME_UNKNOWN_VALUE), 6)];
        ntp_get_formatted_time(current_formatted_time, ARRAY_SIZE(current_formatted_time));
        lv_label_set_text(ui_menu_indicators_current_time_value, current_formatted_time);

        // Reset timer
        last_indicators_update = current_time;
    }
}
