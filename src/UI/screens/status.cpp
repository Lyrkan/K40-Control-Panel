#include <lvgl.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>

#include "K40/alerts.h"
#include "K40/cooling.h"
#include "K40/flame_sensor.h"
#include "K40/lids.h"
#include "UI/images.h"
#include "UI/screens/status.h"
#include "UI/utils.h"
#include "cpu_monitor.h"
#include "macros.h"
#include "mutex.h"
#include "queues.h"

lv_obj_t *ui_status_screen;

static StaticEventGroup_t ui_status_event_group_static;
static EventGroupHandle_t ui_status_event_group = xEventGroupCreateStatic(&ui_status_event_group_static);

static lv_obj_t *ui_status_laser_state_value;
static lv_obj_t *ui_status_laser_alarm_value;
static lv_obj_t *ui_status_cooling_input_flow_bar;
static lv_obj_t *ui_status_cooling_input_flow_value;
static lv_obj_t *ui_status_cooling_input_temp_bar;
static lv_obj_t *ui_status_cooling_input_temp_value;
static lv_obj_t *ui_status_cooling_output_flow_bar;
static lv_obj_t *ui_status_cooling_output_flow_value;
static lv_obj_t *ui_status_cooling_output_temp_bar;
static lv_obj_t *ui_status_cooling_output_temp_value;
static lv_obj_t *ui_status_lid_front_value;
static lv_obj_t *ui_status_lid_back_value;
static lv_obj_t *ui_status_fire_value;
static lv_obj_t *ui_status_uart_status_value;
static lv_obj_t *ui_status_heap_value;
static lv_obj_t *ui_status_cpu_0_value;
static lv_obj_t *ui_status_cpu_1_value;

static void ui_status_init_screen_content() {
    // Make sure the screen is empty
    lv_obj_clean(ui_status_screen);

    lv_obj_t *ui_status_main_panel = ui_utils_create_screen_panel(ui_status_screen);

    // Laser card
    lv_obj_t *ui_laser_card = ui_utils_create_card(ui_status_main_panel, "LASER", &image_laser_head);
    lv_obj_set_width(ui_laser_card, 225);
    lv_obj_set_height(ui_laser_card, 100);
    lv_obj_set_pos(ui_laser_card, 0, 0);

    lv_obj_t *ui_status_laser_labels = lv_label_create(ui_laser_card);
    lv_obj_set_width(ui_status_laser_labels, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_laser_labels, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_laser_labels, 0);
    lv_obj_set_y(ui_status_laser_labels, 35);
    lv_label_set_text(ui_status_laser_labels, "State:\nAlarm:");

    ui_status_laser_state_value = lv_label_create(ui_laser_card);
    lv_obj_set_width(ui_status_laser_state_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_laser_state_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_laser_state_value, 50);
    lv_obj_set_y(ui_status_laser_state_value, 35);
    lv_label_set_text(ui_status_laser_state_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_laser_state_value, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_laser_alarm_value = lv_label_create(ui_laser_card);
    lv_obj_set_width(ui_status_laser_alarm_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_laser_alarm_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_laser_alarm_value, 50);
    lv_obj_set_y(ui_status_laser_alarm_value, 52);
    lv_label_set_text(ui_status_laser_alarm_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_laser_alarm_value, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Lids card
    lv_obj_t *ui_lids_card = ui_utils_create_card(ui_status_main_panel, "LIDS", LV_SYMBOL_EYE, lv_color_hex(0xF94892));
    lv_obj_set_width(ui_lids_card, 225);
    lv_obj_set_height(ui_lids_card, 100);
    lv_obj_set_pos(ui_lids_card, 235, 0);

    lv_obj_t *ui_status_lid_labels = lv_label_create(ui_lids_card);
    lv_obj_set_width(ui_status_lid_labels, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_lid_labels, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_lid_labels, 0);
    lv_obj_set_y(ui_status_lid_labels, 35);
    lv_label_set_text(ui_status_lid_labels, "Front lid:\nBack lid:");

    ui_status_lid_front_value = lv_label_create(ui_lids_card);
    lv_obj_set_width(ui_status_lid_front_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_lid_front_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_lid_front_value, 65);
    lv_obj_set_y(ui_status_lid_front_value, 35);
    lv_label_set_text(ui_status_lid_front_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_lid_front_value, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_lid_back_value = lv_label_create(ui_lids_card);
    lv_obj_set_width(ui_status_lid_back_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_lid_back_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_lid_back_value, 65);
    lv_obj_set_y(ui_status_lid_back_value, 52);
    lv_label_set_text(ui_status_lid_back_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_lid_back_value, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Cooling card
    lv_obj_t *ui_cooling_card =
        ui_utils_create_card(ui_status_main_panel, "COOLING", LV_SYMBOL_SNOWFLAKE, lv_color_hex(0x89CFFD));
    lv_obj_set_width(ui_cooling_card, 225);
    lv_obj_set_height(ui_cooling_card, 145);
    lv_obj_set_pos(ui_cooling_card, 0, 110);

    lv_obj_t *ui_status_cooling_input_label = lv_label_create(ui_cooling_card);
    lv_obj_set_width(ui_status_cooling_input_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cooling_input_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_cooling_input_label, 0);
    lv_obj_set_y(ui_status_cooling_input_label, 35);
    lv_label_set_text(ui_status_cooling_input_label, "Input flow:\nInput temp:");

    ui_status_cooling_input_flow_bar = lv_bar_create(ui_cooling_card);
    lv_obj_set_width(ui_status_cooling_input_flow_bar, 110);
    lv_obj_set_height(ui_status_cooling_input_flow_bar, 10);
    lv_obj_set_x(ui_status_cooling_input_flow_bar, 90);
    lv_obj_set_y(ui_status_cooling_input_flow_bar, 39);

    ui_status_cooling_input_flow_value = lv_label_create(ui_status_cooling_input_flow_bar);
    lv_obj_set_width(ui_status_cooling_input_flow_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cooling_input_flow_value, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_status_cooling_input_flow_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_status_cooling_input_flow_value, "Unknown");
    lv_obj_set_style_text_color(
        ui_status_cooling_input_flow_value,
        lv_color_hex(0x777777),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_cooling_input_flow_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_cooling_input_temp_bar = lv_bar_create(ui_cooling_card);
    lv_obj_set_width(ui_status_cooling_input_temp_bar, 110);
    lv_obj_set_height(ui_status_cooling_input_temp_bar, 10);
    lv_obj_set_x(ui_status_cooling_input_temp_bar, 90);
    lv_obj_set_y(ui_status_cooling_input_temp_bar, 56);

    ui_status_cooling_input_temp_value = lv_label_create(ui_status_cooling_input_temp_bar);
    lv_obj_set_width(ui_status_cooling_input_temp_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cooling_input_temp_value, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_status_cooling_input_temp_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_status_cooling_input_temp_value, "Unknown");
    lv_obj_set_style_text_color(
        ui_status_cooling_input_temp_value,
        lv_color_hex(0x777777),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_cooling_input_temp_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_status_cooling_output_label = lv_label_create(ui_cooling_card);
    lv_obj_set_width(ui_status_cooling_output_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cooling_output_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_cooling_output_label, 0);
    lv_obj_set_y(ui_status_cooling_output_label, 75);
    lv_label_set_text(ui_status_cooling_output_label, "Output flow:\nOutput temp:");

    ui_status_cooling_output_flow_bar = lv_bar_create(ui_cooling_card);
    lv_obj_set_width(ui_status_cooling_output_flow_bar, 110);
    lv_obj_set_height(ui_status_cooling_output_flow_bar, 10);
    lv_obj_set_x(ui_status_cooling_output_flow_bar, 90);
    lv_obj_set_y(ui_status_cooling_output_flow_bar, 79);

    ui_status_cooling_output_flow_value = lv_label_create(ui_status_cooling_output_flow_bar);
    lv_obj_set_width(ui_status_cooling_output_flow_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cooling_output_flow_value, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_status_cooling_output_flow_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_status_cooling_output_flow_value, "Unknown");
    lv_obj_set_style_text_color(
        ui_status_cooling_output_flow_value,
        lv_color_hex(0x777777),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_cooling_output_flow_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_cooling_output_temp_bar = lv_bar_create(ui_cooling_card);
    lv_obj_set_width(ui_status_cooling_output_temp_bar, 110);
    lv_obj_set_height(ui_status_cooling_output_temp_bar, 10);
    lv_obj_set_x(ui_status_cooling_output_temp_bar, 90);
    lv_obj_set_y(ui_status_cooling_output_temp_bar, 96);

    ui_status_cooling_output_temp_value = lv_label_create(ui_status_cooling_output_temp_bar);
    lv_obj_set_width(ui_status_cooling_output_temp_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cooling_output_temp_value, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_status_cooling_output_temp_value, LV_ALIGN_CENTER);
    lv_label_set_text(ui_status_cooling_output_temp_value, "Unknown");
    lv_obj_set_style_text_color(
        ui_status_cooling_output_temp_value,
        lv_color_hex(0x777777),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_status_cooling_output_temp_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Misc. card
    lv_obj_t *ui_misc_card = ui_utils_create_card(ui_status_main_panel, "MISC", LV_SYMBOL_FIRE, lv_color_hex(0xFF7F3F));
    lv_obj_set_width(ui_misc_card, 225);
    lv_obj_set_height(ui_misc_card, 145);
    lv_obj_set_pos(ui_misc_card, 235, 110);

    lv_obj_t *ui_status_misc_labels = lv_label_create(ui_misc_card);
    lv_obj_set_width(ui_status_misc_labels, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_misc_labels, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_misc_labels, 0);
    lv_obj_set_y(ui_status_misc_labels, 35);
    lv_label_set_text(ui_status_misc_labels, "Flame sensor:\nUART:\nCore #0:\nCore #1:\nFree heap:");

    ui_status_fire_value = lv_label_create(ui_misc_card);
    lv_obj_set_width(ui_status_fire_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_fire_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_fire_value, 100);
    lv_obj_set_y(ui_status_fire_value, 35);
    lv_label_set_text(ui_status_fire_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_fire_value, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_uart_status_value = lv_label_create(ui_misc_card);
    lv_obj_set_width(ui_status_uart_status_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_uart_status_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_uart_status_value, 100);
    lv_obj_set_y(ui_status_uart_status_value, 35 + 17);
    lv_label_set_text(ui_status_uart_status_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_uart_status_value, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_cpu_0_value = lv_label_create(ui_misc_card);
    lv_obj_set_width(ui_status_cpu_0_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cpu_0_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_cpu_0_value, 100);
    lv_obj_set_y(ui_status_cpu_0_value, 35 + (17 * 2));
    lv_label_set_text(ui_status_cpu_0_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_cpu_0_value, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_cpu_1_value = lv_label_create(ui_misc_card);
    lv_obj_set_width(ui_status_cpu_1_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_cpu_1_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_cpu_1_value, 100);
    lv_obj_set_y(ui_status_cpu_1_value, 35 + (17 * 3));
    lv_label_set_text(ui_status_cpu_1_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_cpu_1_value, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_status_heap_value = lv_label_create(ui_misc_card);
    lv_obj_set_width(ui_status_heap_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_status_heap_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_status_heap_value, 100);
    lv_obj_set_y(ui_status_heap_value, 35 + (17 * 4));
    lv_label_set_text(ui_status_heap_value, "Unknown");
    lv_obj_set_style_text_color(ui_status_heap_value, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Force the first update
    ui_status_update(true);
};

void ui_status_init() {
    ui_status_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_status_screen, lv_color_hex(0xFAFAFA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_status_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(
        ui_status_screen,
        [](lv_event_t *e) -> void {
            lv_event_code_t event_code = lv_event_get_code(e);
            switch (event_code) {
            case LV_EVENT_SCREEN_LOAD_START:
                ui_status_init_screen_content();
                break;
            case LV_EVENT_SCREEN_UNLOADED:
                lv_obj_clean(ui_status_screen);
                break;
            }
        },
        LV_EVENT_ALL,
        NULL);
}

void ui_status_update(bool initialize) {
    if (!initialize && (lv_scr_act() != ui_status_screen)) {
        return;
    }

    static char cooling_input_flow_formatted_value[10];
    static char cooling_input_temp_formatted_value[10];
    static char cooling_output_flow_formatted_value[10];
    static char cooling_output_temp_formatted_value[10];

    static char heap_status[250];
    static char cpu_status_0[250];
    static char cpu_status_1[250];

    uint8_t pending_updates = xEventGroupGetBits(ui_status_event_group);
    bool pending_cooling_update = initialize || ((pending_updates & STATUS_UPDATE_PROBE_COOLING) != 0);
    bool pending_lids_update = initialize || ((pending_updates & STATUS_UPDATE_PROBE_LIDS) != 0);
    bool pending_flame_sensor_update = initialize || ((pending_updates & STATUS_UPDATE_PROBE_FLAME_SENSOR) != 0);

    uint8_t alerts_status = alerts_get_current_alerts();

    // Update cooling widgets
    if (pending_cooling_update) {
        TAKE_MUTEX(cooling_current_status_mutex)
        // Input Cooling flow
        sprintf(cooling_input_flow_formatted_value, "%2.2fL/mn", cooling_values.input_flow);
        lv_label_set_text(ui_status_cooling_input_flow_value, cooling_input_flow_formatted_value);
        lv_bar_set_value(ui_status_cooling_input_flow_bar, (int)cooling_values.input_flow, LV_ANIM_ON);

        // Input Cooling temperature
        sprintf(cooling_input_temp_formatted_value, "%2.2f°C", cooling_values.input_temperature);
        lv_label_set_text(ui_status_cooling_input_temp_value, cooling_input_temp_formatted_value);
        lv_bar_set_value(ui_status_cooling_input_temp_bar, (int)cooling_values.input_temperature, LV_ANIM_ON);

        // Output Cooling flow
        sprintf(cooling_output_flow_formatted_value, "%2.2fL/mn", cooling_values.output_flow);
        lv_label_set_text(ui_status_cooling_output_flow_value, cooling_output_flow_formatted_value);
        lv_bar_set_value(ui_status_cooling_output_flow_bar, (int)cooling_values.output_flow, LV_ANIM_ON);

        // Output Cooling temperature
        sprintf(cooling_output_temp_formatted_value, "%2.2f°C", cooling_values.output_temperature);
        lv_label_set_text(ui_status_cooling_output_temp_value, cooling_output_temp_formatted_value);
        lv_bar_set_value(ui_status_cooling_output_temp_bar, (int)cooling_values.output_temperature, LV_ANIM_ON);

        xEventGroupClearBits(
            ui_status_event_group,
            STATUS_UPDATE_PROBE_COOLING | STATUS_UPDATE_PROBE_LIDS | STATUS_UPDATE_PROBE_FLAME_SENSOR);
        RELEASE_MUTEX(cooling_current_status_mutex)
    }

    // Update lids widgets
    if (pending_lids_update) {
        TAKE_MUTEX(lids_current_status_mutex)
        lv_label_set_text(ui_status_lid_front_value, lids_states.front_opened ? "Opened" : "Closed");
        lv_label_set_text(ui_status_lid_back_value, lids_states.back_opened ? "Opened" : "Closed");
        xEventGroupClearBits(ui_status_event_group, STATUS_UPDATE_PROBE_LIDS);
        RELEASE_MUTEX(lids_current_status_mutex)
    }

    // Update flame sensor widgets
    if (pending_flame_sensor_update) {
        TAKE_MUTEX(flame_sensor_current_status_mutex)
        lv_label_set_text(ui_status_fire_value, flame_sensor_triggered ? "Triggered" : "OK");
        xEventGroupClearBits(ui_status_event_group, STATUS_UPDATE_PROBE_FLAME_SENSOR);
        RELEASE_MUTEX(flame_sensor_current_status_mutex)
    }

    // Update heap indicator
    static unsigned long system_status_last_update = 0;
    unsigned long current_time = millis();
    if (system_status_last_update == 0) {
        system_status_last_update = current_time;
    }

    unsigned long delta_time = current_time - system_status_last_update;
    if (initialize || (delta_time >= STATUS_SYSTEM_UPDATE_INTERVAL)) {
        snprintf(heap_status, ARRAY_SIZE(heap_status), "%d/%dkB", ESP.getFreeHeap() / 1024, ESP.getHeapSize() / 1024);

        TAKE_MUTEX(cpu_monitor_stats_mutex)
        snprintf(cpu_status_0, ARRAY_SIZE(cpu_status_0), "%.2f%%", cpu_monitor_load_0);
        snprintf(cpu_status_1, ARRAY_SIZE(cpu_status_1), "%.2f%%", cpu_monitor_load_1);
        RELEASE_MUTEX(cpu_monitor_stats_mutex)

        lv_label_set_text(ui_status_heap_value, heap_status);
        lv_label_set_text(ui_status_cpu_0_value, cpu_status_0);
        lv_label_set_text(ui_status_cpu_1_value, cpu_status_1);

        system_status_last_update = current_time;
    }
}

void ui_status_notify_update(uint8_t update_types) { xEventGroupSetBits(ui_status_event_group, update_types); }
