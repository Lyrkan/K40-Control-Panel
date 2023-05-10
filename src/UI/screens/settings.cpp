#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <lvgl.h>
#include <math.h>
#include <WiFi.h>

#include "UI/menu.h"
#include "UI/screens/settings.h"
#include "macros.h"
#include "settings.h"
#include "wifi.h"

lv_obj_t *ui_settings_screen;

static bool settings_loaded = false;

static lv_obj_t *ui_settings_menu;
static lv_obj_t *ui_settings_root_page;
static lv_obj_t *ui_settings_wifi_page;
static lv_obj_t *ui_settings_bed_page;
static lv_obj_t *ui_settings_probes_page;
static lv_obj_t *ui_settings_ota_page;

static lv_obj_t *ui_settings_keyboard;

static lv_obj_t *ui_settings_wifi_page_panel;
static lv_obj_t *ui_settings_wifi_passphrase_value;
static lv_obj_t *ui_settings_wifi_disconnect_button;
static lv_obj_t *ui_settings_wifi_current_ip_label;
static lv_obj_t *ui_settings_wifi_ssid_value;
static lv_obj_t *ui_settings_wifi_connect_button;

static lv_obj_t *ui_settings_bed_screw_pitch_value;
static lv_obj_t *ui_settings_bed_microstep_multiplier_value;
static lv_obj_t *ui_settings_bed_steps_per_revolution_value;
static lv_obj_t *ui_settings_bed_acceleration_value;
static lv_obj_t *ui_settings_bed_moving_speed_value;
static lv_obj_t *ui_settings_bed_homing_speed_value;

static lv_obj_t *ui_settings_probes_v1_min_value;
static lv_obj_t *ui_settings_probes_v1_max_value;
static lv_obj_t *ui_settings_probes_v2_min_value;
static lv_obj_t *ui_settings_probes_v2_max_value;
static lv_obj_t *ui_settings_probes_v3_min_value;
static lv_obj_t *ui_settings_probes_v3_max_value;
static lv_obj_t *ui_settings_probes_cooling_flow_min_value;
static lv_obj_t *ui_settings_probes_cooling_flow_max_value;
static lv_obj_t *ui_settings_probes_cooling_temp_min_value;
static lv_obj_t *ui_settings_probes_cooling_temp_max_value;

static lv_obj_t *ui_settings_ota_login_value;
static lv_obj_t *ui_settings_ota_password_value;

static void ui_settings_load_bed_settings() {
    // Acquire bed settings mutex
    TAKE_MUTEX(bed_settings_mutex)

    static char screw_pitch_text[10];
    static char microstep_multiplier_text[10];
    static char steps_per_revolution_text[10];
    static char acceleration_text[10];
    static char moving_speed_text[10];
    static char homing_speed_text[10];

    snprintf(screw_pitch_text, ARRAY_SIZE(screw_pitch_text), "%.2f", bed_settings.screw_pitch);
    snprintf(microstep_multiplier_text, ARRAY_SIZE(microstep_multiplier_text), "%d", bed_settings.microstep_multiplier);
    snprintf(steps_per_revolution_text, ARRAY_SIZE(steps_per_revolution_text), "%d", bed_settings.steps_per_revolution);
    snprintf(acceleration_text, ARRAY_SIZE(acceleration_text), "%d", bed_settings.acceleration);
    snprintf(moving_speed_text, ARRAY_SIZE(moving_speed_text), "%d", bed_settings.moving_speed);
    snprintf(homing_speed_text, ARRAY_SIZE(homing_speed_text), "%d", bed_settings.homing_speed);

    lv_textarea_set_text(ui_settings_bed_screw_pitch_value, screw_pitch_text);
    lv_textarea_set_text(ui_settings_bed_microstep_multiplier_value, microstep_multiplier_text);
    lv_textarea_set_text(ui_settings_bed_steps_per_revolution_value, steps_per_revolution_text);
    lv_textarea_set_text(ui_settings_bed_acceleration_value, acceleration_text);
    lv_textarea_set_text(ui_settings_bed_moving_speed_value, moving_speed_text);
    lv_textarea_set_text(ui_settings_bed_homing_speed_value, homing_speed_text);

    // Release bed settings mutex
    RELEASE_MUTEX(bed_settings_mutex)
}

static void ui_settings_save_bed_settings() {
    // Acquire bed settings mutex
    TAKE_MUTEX(bed_settings_mutex)

    bed_settings.screw_pitch = static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_bed_screw_pitch_value)));
    bed_settings.microstep_multiplier =
        static_cast<uint32_t>(atoi(lv_textarea_get_text(ui_settings_bed_microstep_multiplier_value)));
    bed_settings.steps_per_revolution =
        static_cast<uint32_t>(atoi(lv_textarea_get_text(ui_settings_bed_steps_per_revolution_value)));
    bed_settings.acceleration = static_cast<uint32_t>(atoi(lv_textarea_get_text(ui_settings_bed_acceleration_value)));
    bed_settings.moving_speed = static_cast<uint32_t>(atoi(lv_textarea_get_text(ui_settings_bed_moving_speed_value)));
    bed_settings.homing_speed = static_cast<uint32_t>(atoi(lv_textarea_get_text(ui_settings_bed_homing_speed_value)));
    settings_schedule_save(SETTINGS_TYPE_BED);

    // Release bed settings mutex
    RELEASE_MUTEX(bed_settings_mutex)
}

static void ui_settings_load_probes_settings() {
    // Acquire probes settings mutex
    TAKE_MUTEX(probes_settings_mutex)

    static char v1_min_text[10];
    static char v1_max_text[10];
    static char v2_min_text[10];
    static char v2_max_text[10];
    static char v3_min_text[10];
    static char v3_max_text[10];
    static char cooling_flow_min_text[10];
    static char cooling_flow_max_text[10];
    static char cooling_temp_min_text[10];
    static char cooling_temp_max_text[10];

    snprintf(v1_min_text, ARRAY_SIZE(v1_min_text), "%.2f", probes_settings.voltage_probe_v1_min);
    snprintf(v1_max_text, ARRAY_SIZE(v1_max_text), "%.2f", probes_settings.voltage_probe_v1_max);
    snprintf(v2_min_text, ARRAY_SIZE(v2_min_text), "%.2f", probes_settings.voltage_probe_v2_min);
    snprintf(v2_max_text, ARRAY_SIZE(v2_max_text), "%.2f", probes_settings.voltage_probe_v2_max);
    snprintf(v3_min_text, ARRAY_SIZE(v3_min_text), "%.2f", probes_settings.voltage_probe_v3_min);
    snprintf(v3_max_text, ARRAY_SIZE(v3_max_text), "%.2f", probes_settings.voltage_probe_v3_max);
    snprintf(cooling_flow_min_text, ARRAY_SIZE(cooling_flow_min_text), "%.2f", probes_settings.cooling_flow_min);
    snprintf(cooling_flow_max_text, ARRAY_SIZE(cooling_flow_max_text), "%.2f", probes_settings.cooling_flow_max);
    snprintf(cooling_temp_min_text, ARRAY_SIZE(cooling_temp_min_text), "%.2f", probes_settings.cooling_temp_min);
    snprintf(cooling_temp_max_text, ARRAY_SIZE(cooling_temp_max_text), "%.2f", probes_settings.cooling_temp_max);

    lv_textarea_set_text(ui_settings_probes_v1_min_value, v1_min_text);
    lv_textarea_set_text(ui_settings_probes_v1_max_value, v1_max_text);
    lv_textarea_set_text(ui_settings_probes_v2_min_value, v2_min_text);
    lv_textarea_set_text(ui_settings_probes_v2_max_value, v2_max_text);
    lv_textarea_set_text(ui_settings_probes_v3_min_value, v3_min_text);
    lv_textarea_set_text(ui_settings_probes_v3_max_value, v3_max_text);
    lv_textarea_set_text(ui_settings_probes_cooling_flow_min_value, cooling_flow_min_text);
    lv_textarea_set_text(ui_settings_probes_cooling_flow_max_value, cooling_flow_max_text);
    lv_textarea_set_text(ui_settings_probes_cooling_temp_min_value, cooling_temp_min_text);
    lv_textarea_set_text(ui_settings_probes_cooling_temp_max_value, cooling_temp_max_text);

    // Release probes settings mutex
    RELEASE_MUTEX(probes_settings_mutex)
}

static void ui_settings_save_probes_settings() {
    // Acquire probes settings mutex
    TAKE_MUTEX(probes_settings_mutex)

    probes_settings.voltage_probe_v1_min =
        static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_probes_v1_min_value)));
    probes_settings.voltage_probe_v1_max =
        static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_probes_v1_max_value)));
    probes_settings.voltage_probe_v2_min =
        static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_probes_v2_min_value)));
    probes_settings.voltage_probe_v2_max =
        static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_probes_v2_max_value)));
    probes_settings.voltage_probe_v3_min =
        static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_probes_v3_min_value)));
    probes_settings.voltage_probe_v3_max =
        static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_probes_v3_max_value)));
    probes_settings.cooling_flow_min =
        static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_probes_cooling_flow_min_value)));
    probes_settings.cooling_flow_max =
        static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_probes_cooling_flow_max_value)));
    probes_settings.cooling_temp_min =
        static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_probes_cooling_temp_min_value)));
    probes_settings.cooling_temp_max =
        static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_probes_cooling_temp_max_value)));
    settings_schedule_save(SETTINGS_TYPE_PROBES);

    // Release bed settings mutex
    RELEASE_MUTEX(probes_settings_mutex)
}

static void ui_settings_load_ota_settings() {
    // Acquire OTA settings mutex
    TAKE_MUTEX(ota_settings_mutex)

    lv_textarea_set_text(ui_settings_ota_login_value, ota_settings.login);
    lv_textarea_set_text(ui_settings_ota_password_value, ota_settings.password);

    // Release OTA settings mutex
    RELEASE_MUTEX(ota_settings_mutex)
}

static void ui_settings_save_ota_settings() {
    // Acquire OTA settings mutex
    TAKE_MUTEX(ota_settings_mutex)

    strncpy(ota_settings.login, lv_textarea_get_text(ui_settings_ota_login_value), ARRAY_SIZE(ota_settings.login));
    strncpy(
        ota_settings.password,
        lv_textarea_get_text(ui_settings_ota_password_value),
        ARRAY_SIZE(ota_settings.password));
    settings_schedule_save(SETTINGS_TYPE_OTA);

    // Release bed settings mutex
    RELEASE_MUTEX(ota_settings_mutex)
}

static void ui_settings_wifi_buttons_handler(lv_event_t *e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code != LV_EVENT_CLICKED) {
        return;
    }

    lv_obj_t *event_target = lv_event_get_target(e);
    if (event_target == ui_settings_wifi_connect_button) {
        const char *ssid = lv_textarea_get_text(ui_settings_wifi_ssid_value);
        const char *passphrase = lv_textarea_get_text(ui_settings_wifi_passphrase_value);
        if (strlen(ssid) > 0 && strlen(passphrase) > 0) {
            lv_obj_add_state(ui_settings_wifi_connect_button, LV_STATE_DISABLED);
            wifi_connect(ssid, passphrase);
        }
    } else if (event_target == ui_settings_wifi_disconnect_button) {
        wifi_disconnect();
    }
}

static void ui_settings_textarea_focused_event_handler(lv_event_t *e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    lv_obj_t *keyboard = (lv_obj_t *)lv_event_get_user_data(e);

    // Position keyboard at the top or the bottom depending on
    // where the targeted field is located.
    lv_area_t target_coords;
    lv_obj_get_coords(target, &target_coords);
    lv_coord_t screen_height = lv_obj_get_height(ui_settings_screen);
    bool keyboard_is_top =
        (target_coords.y1 + (target_coords.y2 - target_coords.y1) / 2) > (MENU_HEIGHT + (screen_height / 2));

    switch (event_code) {
    case LV_EVENT_FOCUSED:
        lv_keyboard_set_textarea(keyboard, target);
        lv_obj_set_align(ui_settings_keyboard, keyboard_is_top ? LV_ALIGN_TOP_MID : LV_ALIGN_BOTTOM_MID);
        lv_obj_set_y(ui_settings_keyboard, keyboard_is_top ? MENU_HEIGHT : 0);
        lv_obj_clear_flag(ui_settings_keyboard, LV_OBJ_FLAG_HIDDEN);
        break;
    case LV_EVENT_DEFOCUSED:
        lv_obj_add_flag(ui_settings_keyboard, LV_OBJ_FLAG_HIDDEN);
        break;
    default:
        // Ignore
        break;
    }
}

static void ui_settings_field_value_changed_handler(lv_event_t *e) {
    if (!settings_loaded) {
        // Don't handle changes if the settings
        // are not loaded yet.
        return;
    }

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        if (target == ui_settings_bed_screw_pitch_value || target == ui_settings_bed_microstep_multiplier_value ||
            target == ui_settings_bed_steps_per_revolution_value || target == ui_settings_bed_acceleration_value ||
            target == ui_settings_bed_moving_speed_value || target == ui_settings_bed_homing_speed_value) {
            ui_settings_save_bed_settings();
        } else if (
            target == ui_settings_probes_v1_min_value || target == ui_settings_probes_v1_max_value ||
            target == ui_settings_probes_v2_min_value || target == ui_settings_probes_v2_max_value ||
            target == ui_settings_probes_v3_min_value || target == ui_settings_probes_v3_max_value ||
            target == ui_settings_probes_cooling_flow_min_value ||
            target == ui_settings_probes_cooling_flow_max_value ||
            target == ui_settings_probes_cooling_temp_min_value ||
            target == ui_settings_probes_cooling_temp_max_value) {
            ui_settings_save_probes_settings();
        } else if (target == ui_settings_ota_login_value || target == ui_settings_ota_password_value) {
            ui_settings_save_ota_settings();
        }
    }
}

static lv_obj_t *ui_settings_create_textarea_field(lv_obj_t *parent, const char *label) {
    lv_obj_t *cont = lv_menu_cont_create(parent);
    lv_obj_set_style_pad_ver(cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *label_obj = lv_label_create(cont);
    lv_obj_set_width(label_obj, lv_pct(20));
    lv_obj_set_height(label_obj, LV_SIZE_CONTENT);
    lv_label_set_text(label_obj, label);

    lv_obj_t *textarea = lv_textarea_create(cont);
    lv_obj_set_width(textarea, lv_pct(80));
    lv_textarea_set_one_line(textarea, true);

    // Center label on the vertical axis
    lv_obj_set_y(label_obj, (lv_obj_get_height(textarea) / 2) - (lv_obj_get_height(label_obj) / 2));

    // Handle focus/defocused events
    lv_obj_add_event_cb(textarea, ui_settings_textarea_focused_event_handler, LV_EVENT_ALL, ui_settings_keyboard);

    // Handle change events
    lv_obj_add_event_cb(textarea, ui_settings_field_value_changed_handler, LV_EVENT_VALUE_CHANGED, NULL);

    return textarea;
}

static void ui_settings_init_screen_content() {
    // Create menu
    ui_settings_menu = lv_menu_create(ui_settings_screen);
    lv_obj_set_width(ui_settings_menu, 460);
    lv_obj_set_height(ui_settings_menu, 255);
    lv_obj_set_x(ui_settings_menu, 10);
    lv_obj_set_y(ui_settings_menu, -10);
    lv_obj_set_align(ui_settings_menu, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_bg_opa(ui_settings_menu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_settings_menu, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Create back button
    lv_obj_t *ui_settings_menu_back_button = lv_menu_get_main_header_back_btn(ui_settings_menu);
    lv_obj_t *ui_settings_menu_back_button_label = lv_label_create(ui_settings_menu_back_button);
    lv_label_set_text(ui_settings_menu_back_button_label, "Back");

    // Create menu pages
    char wifi_page_name[] = "WiFi";
    char bed_page_name[] = "Bed";
    char probes_page_name[] = "Probes";
    char ota_page_name[] = "OTA_updates";

    ui_settings_wifi_page = lv_menu_page_create(ui_settings_menu, wifi_page_name);
    ui_settings_bed_page = lv_menu_page_create(ui_settings_menu, bed_page_name);
    ui_settings_probes_page = lv_menu_page_create(ui_settings_menu, probes_page_name);
    ui_settings_ota_page = lv_menu_page_create(ui_settings_menu, ota_page_name);

    // Disable scroll momentum/elasticity
    lv_obj_clear_flag(ui_settings_wifi_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_wifi_page, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(ui_settings_bed_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_bed_page, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(ui_settings_probes_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_probes_page, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(ui_settings_ota_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_ota_page, LV_OBJ_FLAG_SCROLL_ELASTIC);

    // Root page
    ui_settings_root_page = lv_menu_page_create(ui_settings_menu, NULL);
    lv_menu_set_page(ui_settings_menu, ui_settings_root_page);
    lv_obj_t *item_cont;
    lv_obj_t *item_label;

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, "WiFi");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_wifi_page);

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, "Bed");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_bed_page);

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, "Probes");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_probes_page);

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, "OTA Updates");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_ota_page);

    // Keyboard
    ui_settings_keyboard = lv_keyboard_create(ui_settings_screen);
    lv_obj_set_width(ui_settings_keyboard, lv_pct(100));
    lv_obj_set_height(ui_settings_keyboard, lv_pct(40));
    lv_obj_set_align(ui_settings_keyboard, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_pos(ui_settings_keyboard, 0, 0);
    lv_obj_set_style_bg_color(ui_settings_keyboard, lv_color_hex(0xFAFAFA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_settings_keyboard, &font_default_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_settings_keyboard, LV_OBJ_FLAG_HIDDEN);

    // WiFi Page
    ui_settings_wifi_page_panel = lv_obj_create(ui_settings_wifi_page);
    lv_obj_set_width(ui_settings_wifi_page_panel, lv_pct(100));
    lv_obj_set_height(ui_settings_wifi_page_panel, lv_pct(100));
    lv_obj_set_x(ui_settings_wifi_page_panel, 0);
    lv_obj_set_y(ui_settings_wifi_page_panel, 0);
    lv_obj_set_style_bg_opa(ui_settings_wifi_page_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_settings_wifi_page_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_settings_wifi_page_panel, LV_OBJ_FLAG_SCROLLABLE);

    ui_settings_wifi_ssid_value = lv_textarea_create(ui_settings_wifi_page_panel);
    lv_obj_set_width(ui_settings_wifi_ssid_value, lv_pct(100));
    lv_obj_set_height(ui_settings_wifi_ssid_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_settings_wifi_ssid_value, 0);
    lv_obj_set_y(ui_settings_wifi_ssid_value, 0);
    lv_textarea_set_placeholder_text(ui_settings_wifi_ssid_value, "WIFI SSID");
    lv_textarea_set_one_line(ui_settings_wifi_ssid_value, true);
    lv_textarea_set_text(ui_settings_wifi_ssid_value, WiFi.SSID().c_str());
    lv_obj_add_event_cb(
        ui_settings_wifi_ssid_value,
        ui_settings_textarea_focused_event_handler,
        LV_EVENT_ALL,
        ui_settings_keyboard);

    ui_settings_wifi_passphrase_value = lv_textarea_create(ui_settings_wifi_page_panel);
    lv_obj_set_width(ui_settings_wifi_passphrase_value, lv_pct(100));
    lv_obj_set_height(ui_settings_wifi_passphrase_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_settings_wifi_passphrase_value, 0);
    lv_obj_set_y(ui_settings_wifi_passphrase_value, 43);
    lv_textarea_set_placeholder_text(ui_settings_wifi_passphrase_value, "WIFI Passphrase");
    lv_textarea_set_one_line(ui_settings_wifi_passphrase_value, true);
    lv_textarea_set_password_mode(ui_settings_wifi_passphrase_value, true);
    lv_obj_add_event_cb(
        ui_settings_wifi_passphrase_value,
        ui_settings_textarea_focused_event_handler,
        LV_EVENT_ALL,
        ui_settings_keyboard);

    ui_settings_wifi_connect_button = lv_btn_create(ui_settings_wifi_page_panel);
    lv_obj_set_width(ui_settings_wifi_connect_button, 100);
    lv_obj_set_height(ui_settings_wifi_connect_button, 40);
    lv_obj_set_x(ui_settings_wifi_connect_button, 0);
    lv_obj_set_y(ui_settings_wifi_connect_button, 110);
    lv_obj_set_align(ui_settings_wifi_connect_button, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_flag(ui_settings_wifi_connect_button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_settings_wifi_connect_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ui_settings_wifi_connect_button, ui_settings_wifi_buttons_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *ui_settings_wifi_connect_button_label = lv_label_create(ui_settings_wifi_connect_button);
    lv_obj_set_width(ui_settings_wifi_connect_button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_settings_wifi_connect_button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_settings_wifi_connect_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settings_wifi_connect_button_label, "Connect");

    ui_settings_wifi_disconnect_button = lv_btn_create(ui_settings_wifi_page_panel);
    lv_obj_set_width(ui_settings_wifi_disconnect_button, 100);
    lv_obj_set_height(ui_settings_wifi_disconnect_button, 40);
    lv_obj_set_x(ui_settings_wifi_disconnect_button, 0);
    lv_obj_set_y(ui_settings_wifi_disconnect_button, 110);
    lv_obj_set_align(ui_settings_wifi_disconnect_button, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_flag(ui_settings_wifi_disconnect_button, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_settings_wifi_disconnect_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_settings_wifi_disconnect_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(
        ui_settings_wifi_disconnect_button,
        lv_color_hex(0xE80C0C),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_settings_wifi_disconnect_button, ui_settings_wifi_buttons_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *ui_settings_wifi_disconnect_button_label = lv_label_create(ui_settings_wifi_disconnect_button);
    lv_obj_set_width(ui_settings_wifi_disconnect_button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_settings_wifi_disconnect_button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_settings_wifi_disconnect_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_settings_wifi_disconnect_button_label, "Disconnect");

    ui_settings_wifi_current_ip_label = lv_label_create(ui_settings_wifi_page_panel);
    lv_obj_set_width(ui_settings_wifi_current_ip_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_settings_wifi_current_ip_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_settings_wifi_current_ip_label, LV_ALIGN_BOTTOM_RIGHT);
    lv_label_set_text(ui_settings_wifi_current_ip_label, "");
    lv_obj_set_x(ui_settings_wifi_current_ip_label, 0);
    lv_obj_set_y(ui_settings_wifi_current_ip_label, 0);
    lv_obj_set_style_text_font(ui_settings_wifi_current_ip_label, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(
        ui_settings_wifi_current_ip_label,
        lv_color_hex(0xAAAAAA),
        LV_PART_MAIN | LV_STATE_DEFAULT);

    // Bed page
    ui_settings_bed_screw_pitch_value = ui_settings_create_textarea_field(ui_settings_bed_page, "Screw pitch (mm)");
    ui_settings_bed_microstep_multiplier_value =
        ui_settings_create_textarea_field(ui_settings_bed_page, "Microsteps multiplier");
    ui_settings_bed_steps_per_revolution_value =
        ui_settings_create_textarea_field(ui_settings_bed_page, "Steps per revolution");
    ui_settings_bed_acceleration_value = ui_settings_create_textarea_field(ui_settings_bed_page, "Acceleration");
    ui_settings_bed_moving_speed_value = ui_settings_create_textarea_field(ui_settings_bed_page, "Moving speed");
    ui_settings_bed_homing_speed_value = ui_settings_create_textarea_field(ui_settings_bed_page, "Homing speed");

    // Probes page
    ui_settings_probes_v1_min_value = ui_settings_create_textarea_field(ui_settings_probes_page, "V1 min");
    ui_settings_probes_v1_max_value = ui_settings_create_textarea_field(ui_settings_probes_page, "V1 max");
    ui_settings_probes_v2_min_value = ui_settings_create_textarea_field(ui_settings_probes_page, "V2 min");
    ui_settings_probes_v2_max_value = ui_settings_create_textarea_field(ui_settings_probes_page, "V2 max");
    ui_settings_probes_v3_min_value = ui_settings_create_textarea_field(ui_settings_probes_page, "V3 min");
    ui_settings_probes_v3_max_value = ui_settings_create_textarea_field(ui_settings_probes_page, "V3 max");
    ui_settings_probes_cooling_flow_min_value =
        ui_settings_create_textarea_field(ui_settings_probes_page, "Cool. flow min (L/mn)");
    ui_settings_probes_cooling_flow_max_value =
        ui_settings_create_textarea_field(ui_settings_probes_page, "Cool. flow max (L/mn)");
    ui_settings_probes_cooling_temp_min_value =
        ui_settings_create_textarea_field(ui_settings_probes_page, "Cool. temp min (°C)");
    ui_settings_probes_cooling_temp_max_value =
        ui_settings_create_textarea_field(ui_settings_probes_page, "Cool. temp max (°C)");

    // OTA page
    ui_settings_ota_login_value = ui_settings_create_textarea_field(ui_settings_ota_page, "Login");
    ui_settings_ota_password_value = ui_settings_create_textarea_field(ui_settings_ota_page, "Password");

    // Init fields with current settings
    ui_settings_load_bed_settings();
    ui_settings_load_probes_settings();
    ui_settings_load_ota_settings();
    settings_loaded = true;

    // Force the first update
    ui_settings_update();
}

void ui_settings_init() {
    ui_settings_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_settings_screen, lv_color_hex(0xFAFAFA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_settings_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(
        ui_settings_screen,
        [](lv_event_t *e) -> void {
            lv_event_code_t event_code = lv_event_get_code(e);
            switch (event_code) {
            case LV_EVENT_SCREEN_LOAD_START:
                ui_settings_init_screen_content();
                break;
            case LV_EVENT_SCREEN_UNLOADED:
                lv_obj_clean(ui_settings_screen);
                settings_loaded = false;
                break;
            }
        },
        LV_EVENT_ALL,
        NULL);
}

void ui_settings_update() {
    if (lv_scr_act() != ui_settings_screen) {
        return;
    }

    static unsigned long last_update = 0;
    unsigned long current_time = millis();

    if (current_time - last_update > SETTINGS_STATE_UPDATE_INTERVAL) {
        // Update WiFi indicators
        switch (WiFi.status()) {
        case WL_CONNECTED:
            lv_obj_add_flag(ui_settings_wifi_connect_button, LV_OBJ_FLAG_HIDDEN);      // Hide connect button
            lv_obj_clear_flag(ui_settings_wifi_disconnect_button, LV_OBJ_FLAG_HIDDEN); // Show disconnect button
            break;
        case WL_IDLE_STATUS:
            lv_obj_clear_flag(ui_settings_wifi_connect_button, LV_OBJ_FLAG_HIDDEN);  // Show connect button
            lv_obj_add_flag(ui_settings_wifi_disconnect_button, LV_OBJ_FLAG_HIDDEN); // Hide disconnect button
            lv_obj_add_state(ui_settings_wifi_connect_button, LV_STATE_DISABLED);    // Disable connect button
        case WL_CONNECT_FAILED:
        case WL_CONNECTION_LOST:
        case WL_DISCONNECTED:
            lv_obj_clear_flag(ui_settings_wifi_connect_button, LV_OBJ_FLAG_HIDDEN);  // Show connect button
            lv_obj_add_flag(ui_settings_wifi_disconnect_button, LV_OBJ_FLAG_HIDDEN); // Hide disconnect button
            lv_obj_clear_state(ui_settings_wifi_connect_button, LV_STATE_DISABLED);  // Enable connect button
            break;
        }

        // Update current IP
        lv_label_set_text(ui_settings_wifi_current_ip_label, WiFi.localIP().toString().c_str());

        // Reset timer
        last_update = current_time;
    }
}
