#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <lvgl.h>
#include <math.h>
#include <WiFi.h>

#include "UI/menu.h"
#include "UI/utils.h"
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
static lv_obj_t *ui_settings_grbl_page;
static lv_obj_t *ui_settings_alarm_page;
static lv_obj_t *ui_settings_interlock_page;

static lv_obj_t *ui_settings_keyboard;

static lv_obj_t *ui_settings_wifi_page_panel;
static lv_obj_t *ui_settings_wifi_passphrase_value;
static lv_obj_t *ui_settings_wifi_disconnect_button;
static lv_obj_t *ui_settings_wifi_current_ip_label;
static lv_obj_t *ui_settings_wifi_ssid_value;
static lv_obj_t *ui_settings_wifi_connect_button;

static lv_obj_t *ui_settings_bed_screw_lead_value;
static lv_obj_t *ui_settings_bed_microstep_multiplier_value;
static lv_obj_t *ui_settings_bed_steps_per_revolution_value;
static lv_obj_t *ui_settings_bed_moving_speed_value;
static lv_obj_t *ui_settings_bed_homing_speed_value;
static lv_obj_t *ui_settings_bed_backoff_distance_value;

static lv_obj_t *ui_settings_probes_cooling_flow_min_value;
static lv_obj_t *ui_settings_probes_cooling_flow_max_value;
static lv_obj_t *ui_settings_probes_cooling_temp_min_value;
static lv_obj_t *ui_settings_probes_cooling_temp_max_value;

static lv_obj_t *ui_settings_ota_login_value;
static lv_obj_t *ui_settings_ota_password_value;

static lv_obj_t *ui_settings_grbl_jog_speed_value;
static lv_obj_t *ui_settings_grbl_default_timeout_value;
static lv_obj_t *ui_settings_grbl_homing_timeout_value;

static lv_obj_t *ui_settings_alarm_enable_when_running_value;
static lv_obj_t *ui_settings_alarm_enable_when_not_idling_value;
static lv_obj_t *ui_settings_alarm_enable_when_flame_sensor_triggered_value;
static lv_obj_t *ui_settings_alarm_enable_when_cooling_issue_value;
static lv_obj_t *ui_settings_alarm_enable_when_lid_opened_value;

static lv_obj_t *ui_settings_interlock_disable_when_lid_opened_value;
static lv_obj_t *ui_settings_interlock_disable_when_cooling_issue_value;
static lv_obj_t *ui_settings_interlock_disable_when_flame_sensor_triggered_value;

static void ui_settings_load_bed_settings() {
    // Acquire bed settings mutex
    TAKE_MUTEX(bed_settings_mutex)

    static char screw_lead_text[10];
    static char microstep_multiplier_text[10];
    static char steps_per_revolution_text[10];
    static char moving_speed_text[10];
    static char homing_speed_text[10];
    static char backoff_distance_text[10];

    snprintf(screw_lead_text, ARRAY_SIZE(screw_lead_text), "%.2f", bed_settings.screw_lead_um / 1000.f);
    snprintf(microstep_multiplier_text, ARRAY_SIZE(microstep_multiplier_text), "%d", bed_settings.microstep_multiplier);
    snprintf(steps_per_revolution_text, ARRAY_SIZE(steps_per_revolution_text), "%d", bed_settings.steps_per_revolution);
    snprintf(moving_speed_text, ARRAY_SIZE(moving_speed_text), "%d", bed_settings.moving_speed);
    snprintf(homing_speed_text, ARRAY_SIZE(homing_speed_text), "%d", bed_settings.homing_speed);
    snprintf(
        backoff_distance_text,
        ARRAY_SIZE(backoff_distance_text),
        "%.2f",
        bed_settings.backoff_distance_um / 1000.f);

    lv_textarea_set_text(ui_settings_bed_screw_lead_value, screw_lead_text);
    lv_textarea_set_text(ui_settings_bed_microstep_multiplier_value, microstep_multiplier_text);
    lv_textarea_set_text(ui_settings_bed_steps_per_revolution_value, steps_per_revolution_text);
    lv_textarea_set_text(ui_settings_bed_moving_speed_value, moving_speed_text);
    lv_textarea_set_text(ui_settings_bed_homing_speed_value, homing_speed_text);
    lv_textarea_set_text(ui_settings_bed_backoff_distance_value, backoff_distance_text);

    // Release bed settings mutex
    RELEASE_MUTEX(bed_settings_mutex)
}

static void ui_settings_save_bed_settings() {
    // Acquire bed settings mutex
    TAKE_MUTEX(bed_settings_mutex)

    bed_settings.screw_lead_um =
        static_cast<uint32_t>(1000 * atof(lv_textarea_get_text(ui_settings_bed_screw_lead_value)));
    bed_settings.microstep_multiplier =
        static_cast<uint32_t>(atoi(lv_textarea_get_text(ui_settings_bed_microstep_multiplier_value)));
    bed_settings.steps_per_revolution =
        static_cast<uint32_t>(atoi(lv_textarea_get_text(ui_settings_bed_steps_per_revolution_value)));
    bed_settings.moving_speed = static_cast<uint32_t>(atoi(lv_textarea_get_text(ui_settings_bed_moving_speed_value)));
    bed_settings.homing_speed = static_cast<uint32_t>(atoi(lv_textarea_get_text(ui_settings_bed_homing_speed_value)));
    bed_settings.backoff_distance_um =
        static_cast<uint32_t>(1000 * atoi(lv_textarea_get_text(ui_settings_bed_backoff_distance_value)));
    settings_schedule_save(SETTINGS_TYPE_BED);

    // Release bed settings mutex
    RELEASE_MUTEX(bed_settings_mutex)
}

static void ui_settings_load_probes_settings() {
    // Acquire probes settings mutex
    TAKE_MUTEX(probes_settings_mutex)

    static char cooling_flow_min_text[10];
    static char cooling_flow_max_text[10];
    static char cooling_temp_min_text[10];
    static char cooling_temp_max_text[10];

    snprintf(cooling_flow_min_text, ARRAY_SIZE(cooling_flow_min_text), "%.2f", probes_settings.cooling_flow_min);
    snprintf(cooling_flow_max_text, ARRAY_SIZE(cooling_flow_max_text), "%.2f", probes_settings.cooling_flow_max);
    snprintf(cooling_temp_min_text, ARRAY_SIZE(cooling_temp_min_text), "%.2f", probes_settings.cooling_temp_min);
    snprintf(cooling_temp_max_text, ARRAY_SIZE(cooling_temp_max_text), "%.2f", probes_settings.cooling_temp_max);

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

    strlcpy(ota_settings.login, lv_textarea_get_text(ui_settings_ota_login_value), ARRAY_SIZE(ota_settings.login));
    strlcpy(
        ota_settings.password,
        lv_textarea_get_text(ui_settings_ota_password_value),
        ARRAY_SIZE(ota_settings.password));
    settings_schedule_save(SETTINGS_TYPE_OTA);

    // Release bed settings mutex
    RELEASE_MUTEX(ota_settings_mutex)
}

static void ui_settings_load_relays_settings() {
    // Acquire relays settings mutex
    TAKE_MUTEX(relays_settings_mutex)

    // clang-format off
    // Alarm behavior
    ui_utils_toggle_state(ui_settings_alarm_enable_when_running_value, LV_STATE_CHECKED, (bool)(relays_settings.alarm_behavior & ALARM_ENABLE_WHEN_RUNNING));
    ui_utils_toggle_state(ui_settings_alarm_enable_when_not_idling_value, LV_STATE_CHECKED, (bool)(relays_settings.alarm_behavior & ALARM_ENABLE_WHEN_NOT_IDLING));
    ui_utils_toggle_state(ui_settings_alarm_enable_when_flame_sensor_triggered_value, LV_STATE_CHECKED, (bool)(relays_settings.alarm_behavior & ALARM_ENABLE_WHEN_FLAME_SENSOR_TRIGGERED));
    ui_utils_toggle_state(ui_settings_alarm_enable_when_cooling_issue_value, LV_STATE_CHECKED, (bool)(relays_settings.alarm_behavior & ALARM_ENABLE_WHEN_COOLING_ISSUE));
    ui_utils_toggle_state(ui_settings_alarm_enable_when_lid_opened_value, LV_STATE_CHECKED, (bool)(relays_settings.alarm_behavior & ALARM_ENABLE_WHEN_LID_OPENED));

    // Interlock behavior
    ui_utils_toggle_state(ui_settings_interlock_disable_when_lid_opened_value, LV_STATE_CHECKED, (bool)(relays_settings.interlock_behavior & INTERLOCK_DISABLE_WHEN_LID_OPENED));
    ui_utils_toggle_state(ui_settings_interlock_disable_when_cooling_issue_value, LV_STATE_CHECKED,  (bool)(relays_settings.interlock_behavior & INTERLOCK_DISABLE_WHEN_COOLING_ISSUE));
    ui_utils_toggle_state(ui_settings_interlock_disable_when_flame_sensor_triggered_value, LV_STATE_CHECKED,  (bool)(relays_settings.interlock_behavior & INTERLOCK_DISABLE_WHEN_FLAME_SENSOR_TRIGGERED));
    // clang-format on

    // Release relays settings mutex
    RELEASE_MUTEX(relays_settings_mutex)
}

static void ui_settings_save_relays_settings() {
    // Acquire relays settings mutex
    TAKE_MUTEX(relays_settings_mutex)

    // clang-format off
    relays_settings.alarm_behavior =
        (ALARM_ENABLE_WHEN_COOLING_ISSUE * (int) lv_obj_has_state(ui_settings_alarm_enable_when_cooling_issue_value, LV_STATE_CHECKED)) +
        (ALARM_ENABLE_WHEN_FLAME_SENSOR_TRIGGERED * (int) lv_obj_has_state(ui_settings_alarm_enable_when_flame_sensor_triggered_value, LV_STATE_CHECKED)) +
        (ALARM_ENABLE_WHEN_LID_OPENED * (int) lv_obj_has_state(ui_settings_alarm_enable_when_lid_opened_value, LV_STATE_CHECKED)) +
        (ALARM_ENABLE_WHEN_NOT_IDLING * (int) lv_obj_has_state(ui_settings_alarm_enable_when_not_idling_value, LV_STATE_CHECKED)) +
        (ALARM_ENABLE_WHEN_RUNNING * (int) lv_obj_has_state(ui_settings_alarm_enable_when_running_value, LV_STATE_CHECKED));

    relays_settings.interlock_behavior =
        (INTERLOCK_DISABLE_WHEN_COOLING_ISSUE * (int) lv_obj_has_state(ui_settings_interlock_disable_when_cooling_issue_value, LV_STATE_CHECKED)) +
        (INTERLOCK_DISABLE_WHEN_FLAME_SENSOR_TRIGGERED * (int) lv_obj_has_state(ui_settings_interlock_disable_when_flame_sensor_triggered_value, LV_STATE_CHECKED)) +
        (INTERLOCK_DISABLE_WHEN_LID_OPENED * (int) lv_obj_has_state(ui_settings_interlock_disable_when_lid_opened_value, LV_STATE_CHECKED));
    // clang-format on

    settings_schedule_save(SETTINGS_TYPE_RELAYS);

    // Release GRBL settings mutex
    RELEASE_MUTEX(relays_settings_mutex)
}

static void ui_settings_load_grbl_settings() {
    // Acquire GRBL settings mutex
    TAKE_MUTEX(grbl_settings_mutex)

    static char jog_speed_text[10];
    static char default_timeout_text[10];
    static char homing_timeout_text[10];

    snprintf(jog_speed_text, ARRAY_SIZE(jog_speed_text), "%.1f", grbl_settings.jog_speed);
    snprintf(default_timeout_text, ARRAY_SIZE(default_timeout_text), "%d", grbl_settings.default_timeout_ms);
    snprintf(homing_timeout_text, ARRAY_SIZE(homing_timeout_text), "%d", grbl_settings.homing_timeout_ms);

    lv_textarea_set_text(ui_settings_grbl_jog_speed_value, jog_speed_text);
    lv_textarea_set_text(ui_settings_grbl_default_timeout_value, default_timeout_text);
    lv_textarea_set_text(ui_settings_grbl_homing_timeout_value, homing_timeout_text);

    // Release GRBL settings mutex
    RELEASE_MUTEX(grbl_settings_mutex)
}

static void ui_settings_save_grbl_settings() {
    // Acquire GRBL settings mutex
    TAKE_MUTEX(grbl_settings_mutex)

    grbl_settings.jog_speed = static_cast<float_t>(atof(lv_textarea_get_text(ui_settings_grbl_jog_speed_value)));
    grbl_settings.default_timeout_ms =
        static_cast<uint32_t>(atof(lv_textarea_get_text(ui_settings_grbl_default_timeout_value)));
    grbl_settings.homing_timeout_ms =
        static_cast<uint32_t>(atof(lv_textarea_get_text(ui_settings_grbl_homing_timeout_value)));
    settings_schedule_save(SETTINGS_TYPE_GRBL);

    // Release GRBL settings mutex
    RELEASE_MUTEX(grbl_settings_mutex)
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
        if (target == ui_settings_bed_screw_lead_value || target == ui_settings_bed_microstep_multiplier_value ||
            target == ui_settings_bed_steps_per_revolution_value || target == ui_settings_bed_moving_speed_value ||
            target == ui_settings_bed_homing_speed_value || target == ui_settings_bed_backoff_distance_value) {
            ui_settings_save_bed_settings();
        } else if (
            target == ui_settings_probes_cooling_flow_min_value ||
            target == ui_settings_probes_cooling_flow_max_value ||
            target == ui_settings_probes_cooling_temp_min_value ||
            target == ui_settings_probes_cooling_temp_max_value) {
            ui_settings_save_probes_settings();
        } else if (target == ui_settings_ota_login_value || target == ui_settings_ota_password_value) {
            ui_settings_save_ota_settings();
        } else if (
            target == ui_settings_grbl_jog_speed_value || target == ui_settings_grbl_default_timeout_value ||
            target == ui_settings_grbl_homing_timeout_value) {
            ui_settings_save_grbl_settings();
        } else if (
            target == ui_settings_alarm_enable_when_running_value ||
            target == ui_settings_alarm_enable_when_not_idling_value ||
            target == ui_settings_alarm_enable_when_flame_sensor_triggered_value ||
            target == ui_settings_alarm_enable_when_cooling_issue_value ||
            target == ui_settings_alarm_enable_when_lid_opened_value ||
            target == ui_settings_interlock_disable_when_lid_opened_value ||
            target == ui_settings_interlock_disable_when_cooling_issue_value ||
            target == ui_settings_interlock_disable_when_flame_sensor_triggered_value) {
            ui_settings_save_relays_settings();
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

static lv_obj_t *ui_settings_create_checkbox_field(lv_obj_t *parent, const char *label) {
    lv_obj_t *cont = lv_menu_cont_create(parent);
    lv_obj_set_style_pad_ver(cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *checkbox = lv_checkbox_create(cont);
    lv_obj_set_width(checkbox, lv_pct(100));
    lv_checkbox_set_text(checkbox, label);

    // Handle change events
    lv_obj_add_event_cb(checkbox, ui_settings_field_value_changed_handler, LV_EVENT_VALUE_CHANGED, NULL);

    return checkbox;
}

static void ui_settings_init_screen_content() {
    // Make sure the screen is empty
    lv_obj_clean(ui_settings_screen);

    lv_obj_t *ui_settings_main_panel = ui_utils_create_screen_panel(ui_settings_screen);
    lv_obj_t *ui_settings_main_card = ui_utils_create_card(ui_settings_main_panel, "SETTINGS", LV_SYMBOl_GEAR);
    lv_obj_set_width(ui_settings_main_card, 460);
    lv_obj_set_height(ui_settings_main_card, 255);
    lv_obj_set_pos(ui_settings_main_card, 0, 0);

    // Create menu
    ui_settings_menu = lv_menu_create(ui_settings_main_card);
    lv_obj_set_width(ui_settings_menu, 440);
    lv_obj_set_height(ui_settings_menu, 215);
    lv_obj_set_pos(ui_settings_menu, 0, 10);
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
    char ota_page_name[] = "OTA Updates";
    char grbl_page_name[] = "GRBL";
    char alarm_page_name[] = "Alarm behavior";
    char interlock_page_name[] = "Interlock behavior";

    ui_settings_wifi_page = lv_menu_page_create(ui_settings_menu, wifi_page_name);
    ui_settings_bed_page = lv_menu_page_create(ui_settings_menu, bed_page_name);
    ui_settings_probes_page = lv_menu_page_create(ui_settings_menu, probes_page_name);
    ui_settings_ota_page = lv_menu_page_create(ui_settings_menu, ota_page_name);
    ui_settings_grbl_page = lv_menu_page_create(ui_settings_menu, grbl_page_name);
    ui_settings_alarm_page = lv_menu_page_create(ui_settings_menu, alarm_page_name);
    ui_settings_interlock_page = lv_menu_page_create(ui_settings_menu, interlock_page_name);

    // Disable scroll momentum/elasticity
    lv_obj_clear_flag(ui_settings_wifi_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_wifi_page, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(ui_settings_bed_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_bed_page, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(ui_settings_probes_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_probes_page, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(ui_settings_ota_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_ota_page, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(ui_settings_grbl_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_grbl_page, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(ui_settings_alarm_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_alarm_page, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(ui_settings_interlock_page, LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(ui_settings_interlock_page, LV_OBJ_FLAG_SCROLL_ELASTIC);

    // Root page
    ui_settings_root_page = lv_menu_page_create(ui_settings_menu, NULL);
    lv_menu_set_page(ui_settings_menu, ui_settings_root_page);
    lv_obj_t *item_cont;
    lv_obj_t *item_label;

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, LV_SYMBOL_WIFI " WiFi");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_wifi_page);

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, LV_SYMBOL_ARROW_DOWN_UP_ACROSS_LINE " Bed");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_bed_page);

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, LV_SYMBOL_TEMPERATURE_FULL " Probes");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_probes_page);

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, LV_SYMBOL_CODE_PULL_REQUEST " OTA Updates");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_ota_page);

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, LV_SYMBOL_ARROWS_TO_DOT " GRBL");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_grbl_page);

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, LV_SYMBOL_WARNING " Alarm behavior");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_alarm_page);

    item_cont = lv_menu_cont_create(ui_settings_root_page);
    item_label = lv_label_create(item_cont);
    lv_label_set_text(item_label, LV_SYMBOL_UNLOCK " Interlock behavior");
    lv_menu_set_load_page_event(ui_settings_menu, item_cont, ui_settings_interlock_page);

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

    ui_settings_wifi_connect_button = ui_utils_create_small_button(ui_settings_wifi_page_panel, "Connect", 100);
    lv_obj_set_pos(ui_settings_wifi_connect_button, 0, 95);
    lv_obj_set_align(ui_settings_wifi_connect_button, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_event_cb(ui_settings_wifi_connect_button, ui_settings_wifi_buttons_handler, LV_EVENT_CLICKED, NULL);

    ui_settings_wifi_disconnect_button = ui_utils_create_small_button(ui_settings_wifi_page_panel, "Disconnect", 100);
    lv_obj_set_pos(ui_settings_wifi_disconnect_button, 0, 95);
    lv_obj_set_align(ui_settings_wifi_disconnect_button, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_flag(ui_settings_wifi_disconnect_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(
        ui_settings_wifi_disconnect_button,
        lv_color_hex(0xE80C0C),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_settings_wifi_disconnect_button, ui_settings_wifi_buttons_handler, LV_EVENT_CLICKED, NULL);

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
    ui_settings_bed_screw_lead_value = ui_settings_create_textarea_field(ui_settings_bed_page, "Screw lead (mm)");
    ui_settings_bed_microstep_multiplier_value =
        ui_settings_create_textarea_field(ui_settings_bed_page, "Microsteps multiplier");
    ui_settings_bed_steps_per_revolution_value =
        ui_settings_create_textarea_field(ui_settings_bed_page, "Steps per revolution");
    ui_settings_bed_moving_speed_value = ui_settings_create_textarea_field(ui_settings_bed_page, "Moving speed");
    ui_settings_bed_homing_speed_value = ui_settings_create_textarea_field(ui_settings_bed_page, "Homing speed");
    ui_settings_bed_backoff_distance_value =
        ui_settings_create_textarea_field(ui_settings_bed_page, "Backoff distance (mm)");

    // Probes page
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

    // GRBL page
    ui_settings_grbl_jog_speed_value = ui_settings_create_textarea_field(ui_settings_grbl_page, "Jog speed (mm/s)");
    ui_settings_grbl_default_timeout_value =
        ui_settings_create_textarea_field(ui_settings_grbl_page, "Default timeout (ms)");
    ui_settings_grbl_homing_timeout_value =
        ui_settings_create_textarea_field(ui_settings_grbl_page, "Homing timeout (ms)");

    // Alarm page
    ui_settings_alarm_enable_when_running_value =
        ui_settings_create_checkbox_field(ui_settings_alarm_page, "Enable alarm when Grbl status is 'Running'");
    ui_settings_alarm_enable_when_not_idling_value = ui_settings_create_checkbox_field(
        ui_settings_alarm_page,
        "Enable alarm when Grbl status is different from 'Idling'");
    ui_settings_alarm_enable_when_flame_sensor_triggered_value =
        ui_settings_create_checkbox_field(ui_settings_alarm_page, "Enable alarm when flame sensor is triggered");
    ui_settings_alarm_enable_when_cooling_issue_value =
        ui_settings_create_checkbox_field(ui_settings_alarm_page, "Enable alarm when a cooling issue is detected");
    ui_settings_alarm_enable_when_lid_opened_value =
        ui_settings_create_checkbox_field(ui_settings_alarm_page, "Enable alarm when a lid is opened");

    // Interlock page
    ui_settings_interlock_disable_when_lid_opened_value =
        ui_settings_create_checkbox_field(ui_settings_interlock_page, "Prevent laser from firing when a lid is opened");
    ui_settings_interlock_disable_when_cooling_issue_value = ui_settings_create_checkbox_field(
        ui_settings_interlock_page,
        "Prevent laser from firing when a cooling issue is detected");
    ui_settings_interlock_disable_when_flame_sensor_triggered_value = ui_settings_create_checkbox_field(
        ui_settings_interlock_page,
        "Prevent laser from firing when flame sensor is triggered");

    // Init fields with current settings
    ui_settings_load_bed_settings();
    ui_settings_load_probes_settings();
    ui_settings_load_ota_settings();
    ui_settings_load_grbl_settings();
    ui_settings_load_relays_settings();
    settings_loaded = true;

    // Force the first update
    ui_settings_update(true);
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

void ui_settings_update(bool initialize) {
    if (!initialize && (lv_scr_act() != ui_settings_screen)) {
        return;
    }

    static unsigned long last_update = 0;
    unsigned long current_time = millis();
    if (last_update == 0) {
        last_update = current_time;
    }

    unsigned long delta_time = current_time - last_update;

    if (initialize || (delta_time > SETTINGS_STATE_UPDATE_INTERVAL)) {
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
