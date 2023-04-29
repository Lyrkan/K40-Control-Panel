#include <lvgl.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>

#include "K40/alerts.h"
#include "K40/cooling.h"
#include "K40/lids.h"
#include "K40/voltage_probes.h"
#include "UI/images.h"
#include "UI/screens/status.h"
#include "cpu_monitor.h"
#include "macros.h"
#include "queues.h"

lv_obj_t *ui_status_screen;

static StaticEventGroup_t ui_status_event_group_static;
static EventGroupHandle_t ui_status_event_group = xEventGroupCreateStatic(&ui_status_event_group_static);

static lv_obj_t *ui_status_main_panel;
static lv_obj_t *ui_status_voltages_icon;
static lv_obj_t *ui_status_voltages_icon_warning;
static lv_obj_t *ui_status_voltages_label;
static lv_obj_t *ui_status_voltages_v1_bar;
static lv_obj_t *ui_status_voltages_v1_value;
static lv_obj_t *ui_status_voltages_v2_bar;
static lv_obj_t *ui_status_voltages_v2_value;
static lv_obj_t *ui_status_voltages_v3_bar;
static lv_obj_t *ui_status_voltages_v3_value;
static lv_obj_t *ui_status_cooling_icon;
static lv_obj_t *ui_status_cooling_icon_warning;
static lv_obj_t *ui_status_cooling_label;
static lv_obj_t *ui_status_cooling_flow_bar;
static lv_obj_t *ui_status_cooling_flow_value;
static lv_obj_t *ui_status_cooling_temp_bar;
static lv_obj_t *ui_status_cooling_temp_value;
static lv_obj_t *ui_status_lid_icon;
static lv_obj_t *ui_status_lid_icon_warning;
static lv_obj_t *ui_status_lid_label;
static lv_obj_t *ui_status_lid_front_value;
static lv_obj_t *ui_status_lid_back_value;
static lv_obj_t *ui_status_fire_icon;
static lv_obj_t *ui_status_fire_icon_warning;
static lv_obj_t *ui_status_fire_label;
static lv_obj_t *ui_status_fire_value;
static lv_obj_t *ui_status_heap;
static lv_obj_t *ui_status_cpu_0;
static lv_obj_t *ui_status_cpu_1;

void ui_status_init() {
    ui_status_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_status_screen, lv_color_hex(0xFAFAFA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_status_screen, LV_OBJ_FLAG_SCROLLABLE);

    ui_status_main_panel = lv_obj_create(ui_status_screen);
    lv_obj_set_width(ui_status_main_panel, 460);
    lv_obj_set_height(ui_status_main_panel, 255);
    lv_obj_set_x(ui_status_main_panel, 10);
    lv_obj_set_y(ui_status_main_panel, -10);
    lv_obj_set_align(ui_status_main_panel, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_bg_opa(ui_status_main_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_status_main_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_status_main_panel, LV_OBJ_FLAG_SCROLLABLE);

    ui_status_voltages_icon = lv_label_create(ui_status_main_panel);
    lv_label_set_text(ui_status_voltages_icon, LV_SYMBOL_BOLT);
    lv_obj_set_style_text_font(ui_status_voltages_icon, &font_icons_40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_status_voltages_icon, lv_color_hex(0xFBDF07), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_status_voltages_icon, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_status_voltages_icon, 54);
    lv_obj_set_height(ui_status_voltages_icon, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_status_voltages_icon, 0, 0);

    ui_status_voltages_icon_warning = lv_img_create(ui_status_main_panel);
    lv_img_set_src(ui_status_voltages_icon_warning, &icon_warning);
    lv_obj_set_width(ui_status_voltages_icon_warning, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_voltages_icon_warning, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_voltages_icon_warning, 25);
    lv_obj_set_y(ui_status_voltages_icon_warning, 16);

    ui_status_voltages_label = lv_label_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_voltages_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_voltages_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_voltages_label, 70);
    lv_obj_set_y(ui_status_voltages_label, 0);
    lv_label_set_text(ui_status_voltages_label, "V1:\nV2:\nV3:");

    ui_status_voltages_v1_bar = lv_bar_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_voltages_v1_bar, 150);
    lv_obj_set_height(ui_status_voltages_v1_bar, 10);
    lv_obj_set_x(ui_status_voltages_v1_bar, 120);
    lv_obj_set_y(ui_status_voltages_v1_bar, 4);

    ui_status_voltages_v1_value = lv_label_create(ui_status_voltages_v1_bar);
    lv_obj_set_width(ui_status_voltages_v1_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_voltages_v1_value, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_status_voltages_v1_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_status_voltages_v1_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_voltages_v1_value, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_voltages_v1_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_voltages_v2_bar = lv_bar_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_voltages_v2_bar, 150);
    lv_obj_set_height(ui_status_voltages_v2_bar, 10);
    lv_obj_set_x(ui_status_voltages_v2_bar, 120);
    lv_obj_set_y(ui_status_voltages_v2_bar, 21);

    ui_status_voltages_v2_value = lv_label_create(ui_status_voltages_v2_bar);
    lv_obj_set_width(ui_status_voltages_v2_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_voltages_v2_value, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_status_voltages_v2_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_status_voltages_v2_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_voltages_v2_value, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_voltages_v2_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_voltages_v3_bar = lv_bar_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_voltages_v3_bar, 150);
    lv_obj_set_height(ui_status_voltages_v3_bar, 10);
    lv_obj_set_x(ui_status_voltages_v3_bar, 120);
    lv_obj_set_y(ui_status_voltages_v3_bar, 38);

    ui_status_voltages_v3_value = lv_label_create(ui_status_voltages_v3_bar);
    lv_obj_set_width(ui_status_voltages_v3_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_voltages_v3_value, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_status_voltages_v3_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_status_voltages_v3_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_voltages_v3_value, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_voltages_v3_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_cooling_icon = lv_label_create(ui_status_main_panel);
    lv_label_set_text(ui_status_cooling_icon, LV_SYMBOL_SNOWFLAKE);
    lv_obj_set_style_text_font(ui_status_cooling_icon, &font_icons_40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_status_cooling_icon, lv_color_hex(0x89CFFD), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_status_cooling_icon, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_status_cooling_icon, 54);
    lv_obj_set_height(ui_status_cooling_icon, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_status_cooling_icon, 0, 58);

    ui_status_cooling_icon_warning = lv_img_create(ui_status_main_panel);
    lv_img_set_src(ui_status_cooling_icon_warning, &icon_warning);
    lv_obj_set_width(ui_status_cooling_icon_warning, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cooling_icon_warning, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_cooling_icon_warning, 25);
    lv_obj_set_y(ui_status_cooling_icon_warning, 75);

    ui_status_cooling_label = lv_label_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_cooling_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cooling_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_cooling_label, 70);
    lv_obj_set_y(ui_status_cooling_label, 65);
    lv_label_set_text(ui_status_cooling_label, "Flow:\nTemp:");

    ui_status_cooling_flow_bar = lv_bar_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_cooling_flow_bar, 150);
    lv_obj_set_height(ui_status_cooling_flow_bar, 10);
    lv_obj_set_x(ui_status_cooling_flow_bar, 120);
    lv_obj_set_y(ui_status_cooling_flow_bar, 69);

    ui_status_cooling_flow_value = lv_label_create(ui_status_cooling_flow_bar);
    lv_obj_set_width(ui_status_cooling_flow_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cooling_flow_value, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_status_cooling_flow_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_status_cooling_flow_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_cooling_flow_value, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_cooling_flow_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_cooling_temp_bar = lv_bar_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_cooling_temp_bar, 150);
    lv_obj_set_height(ui_status_cooling_temp_bar, 10);
    lv_obj_set_x(ui_status_cooling_temp_bar, 120);
    lv_obj_set_y(ui_status_cooling_temp_bar, 86);

    ui_status_cooling_temp_value = lv_label_create(ui_status_cooling_temp_bar);
    lv_obj_set_width(ui_status_cooling_temp_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cooling_temp_value, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_status_cooling_temp_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_status_cooling_temp_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_cooling_temp_value, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_cooling_temp_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_lid_icon = lv_label_create(ui_status_main_panel);
    lv_label_set_text(ui_status_lid_icon, LV_SYMBOL_EYE);
    lv_obj_set_style_text_font(ui_status_lid_icon, &font_icons_40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_status_lid_icon, lv_color_hex(0xF94892), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_status_lid_icon, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_status_lid_icon, 54);
    lv_obj_set_height(ui_status_lid_icon, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_status_lid_icon, 0, 116);

    ui_status_lid_icon_warning = lv_img_create(ui_status_main_panel);
    lv_img_set_src(ui_status_lid_icon_warning, &icon_warning);
    lv_obj_set_width(ui_status_lid_icon_warning, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_lid_icon_warning, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_lid_icon_warning, 25);
    lv_obj_set_y(ui_status_lid_icon_warning, 133);

    ui_status_lid_label = lv_label_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_lid_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_lid_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_lid_label, 70);
    lv_obj_set_y(ui_status_lid_label, 125);
    lv_label_set_text(ui_status_lid_label, "Front:\nBack:");

    ui_status_lid_front_value = lv_label_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_lid_front_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_lid_front_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_lid_front_value, 115);
    lv_obj_set_y(ui_status_lid_front_value, 126);
    lv_label_set_text(ui_status_lid_front_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_lid_front_value, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_lid_back_value = lv_label_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_lid_back_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_lid_back_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_lid_back_value, 115);
    lv_obj_set_y(ui_status_lid_back_value, 143);
    lv_label_set_text(ui_status_lid_back_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_lid_back_value, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_fire_icon = lv_label_create(ui_status_main_panel);
    lv_label_set_text(ui_status_fire_icon, LV_SYMBOL_FIRE);
    lv_obj_set_style_text_font(ui_status_fire_icon, &font_icons_40, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_status_fire_icon, lv_color_hex(0xFF7F3F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_status_fire_icon, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_width(ui_status_fire_icon, 54);
    lv_obj_set_height(ui_status_fire_icon, LV_SIZE_CONTENT);
    lv_obj_set_pos(ui_status_fire_icon, 0, 174);

    ui_status_fire_icon_warning = lv_img_create(ui_status_main_panel);
    lv_img_set_src(ui_status_fire_icon_warning, &icon_warning);
    lv_obj_set_width(ui_status_fire_icon_warning, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_fire_icon_warning, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_fire_icon_warning, 25);
    lv_obj_set_y(ui_status_fire_icon_warning, 191);

    ui_status_fire_label = lv_label_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_fire_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_fire_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_fire_label, 70);
    lv_obj_set_y(ui_status_fire_label, 190);
    lv_label_set_text(ui_status_fire_label, "Flame sensor:");

    ui_status_fire_value = lv_label_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_fire_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_fire_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_fire_value, 164);
    lv_obj_set_y(ui_status_fire_value, 191);
    lv_label_set_text(ui_status_fire_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_fire_value, lv_color_hex(0x777777), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_cpu_0 = lv_label_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_cpu_0, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cpu_0, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_cpu_0, 0);
    lv_obj_set_y(ui_status_cpu_0, -40);
    lv_obj_set_align(ui_status_cpu_0, LV_ALIGN_BOTTOM_RIGHT);
    lv_label_set_text(ui_status_cpu_0, "");
    lv_obj_set_style_text_color(ui_status_cpu_0, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_cpu_0, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_cpu_1 = lv_label_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_cpu_1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cpu_1, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_cpu_1, 0);
    lv_obj_set_y(ui_status_cpu_1, -20);
    lv_obj_set_align(ui_status_cpu_1, LV_ALIGN_BOTTOM_RIGHT);
    lv_label_set_text(ui_status_cpu_1, "");
    lv_obj_set_style_text_color(ui_status_cpu_1, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_cpu_1, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_heap = lv_label_create(ui_status_main_panel);
    lv_obj_set_width(ui_status_heap, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_heap, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_heap, 0);
    lv_obj_set_y(ui_status_heap, 0);
    lv_obj_set_align(ui_status_heap, LV_ALIGN_BOTTOM_RIGHT);
    lv_label_set_text(ui_status_heap, "");
    lv_obj_set_style_text_color(ui_status_heap, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_heap, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void updateWarningIcon(lv_obj_t *warning_icon, bool show) {
    if (show) {
        if (lv_obj_has_flag(warning_icon, LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_clear_flag(warning_icon, LV_OBJ_FLAG_HIDDEN);
        }
    } else {
        if (!lv_obj_has_flag(warning_icon, LV_OBJ_FLAG_HIDDEN)) {
            lv_obj_add_flag(warning_icon, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

void ui_status_update() {
    static VoltageProbesValues voltage_probes_values;
    static CoolingValues cooling_values;
    static LidsStates lids_states;
    static bool flame_sensor_triggered;

    static char cooling_flow_formatted_value[10];
    static char cooling_temp_formatted_value[10];
    static char voltage_probe1_formatted_value[10];
    static char voltage_probe2_formatted_value[10];
    static char voltage_probe3_formatted_value[10];

    static char heap_status[250];
    static char cpu_status_0[250];
    static char cpu_status_1[250];

    uint8_t pending_updates = xEventGroupGetBits(ui_status_event_group);
    bool pending_voltage_update = (pending_updates & STATUS_UPDATE_PROBE_VOLTAGE) != 0;
    bool pending_cooling_update = (pending_updates & STATUS_UPDATE_PROBE_COOLING) != 0;
    bool pending_lids_update = (pending_updates & STATUS_UPDATE_PROBE_LIDS) != 0;
    bool pending_flame_sensor_update = (pending_updates & STATUS_UPDATE_PROBE_FLAME_SENSOR) != 0;

    uint8_t alerts_statuses = alerts_get_current_alerts();

    // Update voltage probes widgets
    if (pending_voltage_update && xQueuePeek(voltage_current_status_queue, &voltage_probes_values, 0) == pdTRUE) {
        // Probe 1
        sprintf(voltage_probe1_formatted_value, "%2.1fV", voltage_probes_values.probe1);
        lv_label_set_text(ui_status_voltages_v1_value, voltage_probe1_formatted_value);
        lv_bar_set_value(ui_status_voltages_v1_bar, (int)voltage_probes_values.probe1, LV_ANIM_ON);

        // Probe 2
        sprintf(voltage_probe2_formatted_value, "%2.1fV", voltage_probes_values.probe2);
        lv_label_set_text(ui_status_voltages_v2_value, voltage_probe2_formatted_value);
        lv_bar_set_value(ui_status_voltages_v2_bar, (int)voltage_probes_values.probe2, LV_ANIM_ON);

        // Probe 3
        sprintf(voltage_probe3_formatted_value, "%2.1fV", voltage_probes_values.probe3);
        lv_label_set_text(ui_status_voltages_v3_value, voltage_probe3_formatted_value);
        lv_bar_set_value(ui_status_voltages_v3_bar, (int)voltage_probes_values.probe3, LV_ANIM_ON);

        updateWarningIcon(ui_status_voltages_icon_warning, (alerts_statuses & ALERT_TYPE_VOLTAGE) != 0);
    }

    // Update cooling widgets
    if (pending_cooling_update && xQueuePeek(cooling_current_status_queue, &cooling_values, 0) == pdTRUE) {
        // Cooling flow
        sprintf(cooling_flow_formatted_value, "%2.2fL/mn", cooling_values.flow);
        lv_label_set_text(ui_status_cooling_flow_value, cooling_flow_formatted_value);
        lv_bar_set_value(ui_status_cooling_flow_bar, (int)cooling_values.flow, LV_ANIM_ON);

        // Cooling temperature
        sprintf(cooling_temp_formatted_value, "%2.2f°C", cooling_values.temperature);
        lv_label_set_text(ui_status_cooling_temp_value, cooling_temp_formatted_value);
        lv_bar_set_value(ui_status_cooling_temp_bar, (int)cooling_values.temperature, LV_ANIM_ON);

        updateWarningIcon(ui_status_cooling_icon_warning, (alerts_statuses & ALERT_TYPE_COOLING) != 0);
    }

    // Update lids widgets
    if (pending_lids_update && xQueuePeek(lids_current_status_queue, &lids_states, 0) == pdTRUE) {
        lv_label_set_text(ui_status_lid_front_value, lids_states.front_opened ? "Opened" : "Closed");
        lv_label_set_text(ui_status_lid_back_value, lids_states.back_opened ? "Opened" : "Closed");
        updateWarningIcon(ui_status_lid_icon_warning, (alerts_statuses & ALERT_TYPE_LIDS) != 0);
    }

    // Update flame sensor widgets
    if (pending_flame_sensor_update &&
        xQueuePeek(flame_sensor_current_status_queue, &flame_sensor_triggered, 0) == pdTRUE) {
        lv_label_set_text(ui_status_fire_value, flame_sensor_triggered ? "Triggered" : "OK");
        updateWarningIcon(ui_status_fire_icon_warning, (alerts_statuses & ALERT_TYPE_FLAME_SENSOR) != 0);
    }

    // Clear pending updates bits
    xEventGroupClearBits(
        ui_status_event_group,
        (STATUS_UPDATE_PROBE_VOLTAGE | STATUS_UPDATE_PROBE_COOLING | STATUS_UPDATE_PROBE_LIDS |
         STATUS_UPDATE_PROBE_FLAME_SENSOR));

    // Update heap indicator
    static unsigned long system_status_last_update = 0;
    unsigned long current_time = millis();
    if (system_status_last_update == 0) {
        system_status_last_update = current_time;
    }

    unsigned long delta_time = current_time - system_status_last_update;
    if (delta_time >= STATUS_SYSTEM_UPDATE_INTERVAL) {
        snprintf(
            heap_status,
            ARRAY_SIZE(heap_status),
            "Heap: %d/%dkB",
            ESP.getFreeHeap() / 1024,
            ESP.getHeapSize() / 1024);

        xSemaphoreTake(cpu_monitor_stats_mutex, portMAX_DELAY);
        snprintf(cpu_status_0, ARRAY_SIZE(cpu_status_0), "Core #0: %.2f%%", cpu_monitor_load_0);
        snprintf(cpu_status_1, ARRAY_SIZE(cpu_status_1), "Core #1: %.2f%%", cpu_monitor_load_1);
        xSemaphoreGive(cpu_monitor_stats_mutex);

        lv_label_set_text(ui_status_heap, heap_status);
        lv_label_set_text(ui_status_cpu_0, cpu_status_0);
        lv_label_set_text(ui_status_cpu_1, cpu_status_1);

        system_status_last_update = current_time;
    }
}

void ui_status_notify_update(uint8_t update_types) { xEventGroupSetBits(ui_status_event_group, update_types); }
