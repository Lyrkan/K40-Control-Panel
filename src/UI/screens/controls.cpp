#include <Arduino.h>
#include <lvgl.h>

#include "K40/relays.h"
#include "UI/screens/controls.h"
#include "queues.h"

lv_obj_t *ui_controls_screen;

static lv_obj_t *ui_controls_laser_switch;
static lv_obj_t *ui_controls_air_assist_switch;
static lv_obj_t *ui_controls_cooling_switch;
static lv_obj_t *ui_controls_lights_switch;
static lv_obj_t *ui_controls_preview_switch;

static void ui_controls_switch_handler(lv_event_t *e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code != LV_EVENT_VALUE_CHANGED) {
        return;
    }

    lv_obj_t *target = lv_event_get_target(e);

    int32_t relay_pin = -1;
    if (target == ui_controls_laser_switch) {
        relay_pin = PIN_RELAY_LASER;
    } else if (target == ui_controls_air_assist_switch) {
        relay_pin = PIN_RELAY_AIR_ASSIST;
    } else if (target == ui_controls_cooling_switch) {
        relay_pin = PIN_RELAY_COOLING;
    } else if (target == ui_controls_lights_switch) {
        relay_pin = PIN_RELAY_LIGHTS;
    } else if (target == ui_controls_preview_switch) {
        relay_pin = PIN_RELAY_BEAM_PREVIEW;
    }

    if (relay_pin != -1) {
        RelaysCommand command = {
            .pin = (RelayPin)relay_pin,
            .enable = lv_obj_has_state(target, LV_STATE_CHECKED),
        };

        xQueueSendToBack(relays_command_queue, &command, 100);
    }
}

void ui_controls_init_screen_content() {
    // Make sure the screen is empty
    lv_obj_clean(ui_controls_screen);

    lv_obj_t *ui_controls_main_panel = lv_obj_create(ui_controls_screen);
    lv_obj_set_width(ui_controls_main_panel, 460);
    lv_obj_set_height(ui_controls_main_panel, 255);
    lv_obj_set_x(ui_controls_main_panel, 10);
    lv_obj_set_y(ui_controls_main_panel, -10);
    lv_obj_set_align(ui_controls_main_panel, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_bg_opa(ui_controls_main_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_controls_main_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_controls_main_panel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *ui_controls_laser_switch_label = lv_label_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_laser_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_laser_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_laser_switch_label, 0);
    lv_obj_set_y(ui_controls_laser_switch_label, -80);
    lv_obj_set_align(ui_controls_laser_switch_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_controls_laser_switch_label, "Laser");

    lv_obj_t *ui_controls_air_assist_switch_label = lv_label_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_air_assist_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_air_assist_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_air_assist_switch_label, 0);
    lv_obj_set_y(ui_controls_air_assist_switch_label, -40);
    lv_obj_set_align(ui_controls_air_assist_switch_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_controls_air_assist_switch_label, "Air Assist");

    lv_obj_t *ui_controls_cooling_switch_label = lv_label_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_cooling_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_cooling_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_controls_cooling_switch_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_controls_cooling_switch_label, "Cooling");

    lv_obj_t *ui_controls_lights_switch_label = lv_label_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_lights_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_lights_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_lights_switch_label, 0);
    lv_obj_set_y(ui_controls_lights_switch_label, 40);
    lv_obj_set_align(ui_controls_lights_switch_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_controls_lights_switch_label, "Lights");

    lv_obj_t *ui_controls_preview_switch_label = lv_label_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_preview_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_preview_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_preview_switch_label, 0);
    lv_obj_set_y(ui_controls_preview_switch_label, 80);
    lv_obj_set_align(ui_controls_preview_switch_label, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_controls_preview_switch_label, "Diodes");

    ui_controls_laser_switch = lv_switch_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_laser_switch, 50);
    lv_obj_set_height(ui_controls_laser_switch, 25);
    lv_obj_set_x(ui_controls_laser_switch, 80);
    lv_obj_set_y(ui_controls_laser_switch, -80);
    lv_obj_set_align(ui_controls_laser_switch, LV_ALIGN_LEFT_MID);
    lv_obj_add_event_cb(ui_controls_laser_switch, ui_controls_switch_handler, LV_EVENT_VALUE_CHANGED, NULL);

    ui_controls_air_assist_switch = lv_switch_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_air_assist_switch, 50);
    lv_obj_set_height(ui_controls_air_assist_switch, 25);
    lv_obj_set_x(ui_controls_air_assist_switch, 80);
    lv_obj_set_y(ui_controls_air_assist_switch, -40);
    lv_obj_set_align(ui_controls_air_assist_switch, LV_ALIGN_LEFT_MID);
    lv_obj_add_event_cb(ui_controls_air_assist_switch, ui_controls_switch_handler, LV_EVENT_VALUE_CHANGED, NULL);

    ui_controls_cooling_switch = lv_switch_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_cooling_switch, 50);
    lv_obj_set_height(ui_controls_cooling_switch, 25);
    lv_obj_set_x(ui_controls_cooling_switch, 80);
    lv_obj_set_y(ui_controls_cooling_switch, 0);
    lv_obj_set_align(ui_controls_cooling_switch, LV_ALIGN_LEFT_MID);
    lv_obj_add_event_cb(ui_controls_cooling_switch, ui_controls_switch_handler, LV_EVENT_VALUE_CHANGED, NULL);

    ui_controls_lights_switch = lv_switch_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_lights_switch, 50);
    lv_obj_set_height(ui_controls_lights_switch, 25);
    lv_obj_set_x(ui_controls_lights_switch, 80);
    lv_obj_set_y(ui_controls_lights_switch, 40);
    lv_obj_set_align(ui_controls_lights_switch, LV_ALIGN_LEFT_MID);
    lv_obj_add_event_cb(ui_controls_lights_switch, ui_controls_switch_handler, LV_EVENT_VALUE_CHANGED, NULL);

    ui_controls_preview_switch = lv_switch_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_preview_switch, 50);
    lv_obj_set_height(ui_controls_preview_switch, 25);
    lv_obj_set_x(ui_controls_preview_switch, 80);
    lv_obj_set_y(ui_controls_preview_switch, 80);
    lv_obj_set_align(ui_controls_preview_switch, LV_ALIGN_LEFT_MID);
    lv_obj_add_event_cb(ui_controls_preview_switch, ui_controls_switch_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *ui_controls_laser_switch_explanation = lv_label_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_laser_switch_explanation, 199);
    lv_obj_set_height(ui_controls_laser_switch_explanation, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_laser_switch_explanation, 140);
    lv_obj_set_y(ui_controls_laser_switch_explanation, -80);
    lv_obj_set_align(ui_controls_laser_switch_explanation, LV_ALIGN_LEFT_MID);
    lv_label_set_text(
        ui_controls_laser_switch_explanation,
        "Allow laser to be fired.\nCan only be enabled if cooling is active and all lids are closed.");
    lv_obj_set_style_text_color(
        ui_controls_laser_switch_explanation,
        lv_color_hex(0xEA0303),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_controls_laser_switch_explanation, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_controls_air_assist_switch_explanation = lv_label_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_air_assist_switch_explanation, 199);
    lv_obj_set_height(ui_controls_air_assist_switch_explanation, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_air_assist_switch_explanation, 140);
    lv_obj_set_y(ui_controls_air_assist_switch_explanation, -40);
    lv_obj_set_align(ui_controls_air_assist_switch_explanation, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_controls_air_assist_switch_explanation, "Enable air assist pump");
    lv_obj_set_style_text_color(
        ui_controls_air_assist_switch_explanation,
        lv_color_hex(0x808080),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(
        ui_controls_air_assist_switch_explanation,
        &font_default_12,
        LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_controls_cooling_switch_explanation = lv_label_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_cooling_switch_explanation, 199);
    lv_obj_set_height(ui_controls_cooling_switch_explanation, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_cooling_switch_explanation, 140);
    lv_obj_set_y(ui_controls_cooling_switch_explanation, 0);
    lv_obj_set_align(ui_controls_cooling_switch_explanation, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_controls_cooling_switch_explanation, "Enable cooling pump");
    lv_obj_set_style_text_color(
        ui_controls_cooling_switch_explanation,
        lv_color_hex(0x808080),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(
        ui_controls_cooling_switch_explanation,
        &font_default_12,
        LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_controls_lights_switch_explanation = lv_label_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_lights_switch_explanation, 199);
    lv_obj_set_height(ui_controls_lights_switch_explanation, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_lights_switch_explanation, 140);
    lv_obj_set_y(ui_controls_lights_switch_explanation, 40);
    lv_obj_set_align(ui_controls_lights_switch_explanation, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_controls_lights_switch_explanation, "Enable inner lights");
    lv_obj_set_style_text_color(
        ui_controls_lights_switch_explanation,
        lv_color_hex(0x808080),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(
        ui_controls_lights_switch_explanation,
        &font_default_12,
        LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_controls_preview_switch_explanation = lv_label_create(ui_controls_main_panel);
    lv_obj_set_width(ui_controls_preview_switch_explanation, 199);
    lv_obj_set_height(ui_controls_preview_switch_explanation, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_preview_switch_explanation, 140);
    lv_obj_set_y(ui_controls_preview_switch_explanation, 80);
    lv_obj_set_align(ui_controls_preview_switch_explanation, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_controls_preview_switch_explanation, "Enable positioning laser diodes");
    lv_obj_set_style_text_color(
        ui_controls_preview_switch_explanation,
        lv_color_hex(0x808080),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(
        ui_controls_preview_switch_explanation,
        &font_default_12,
        LV_PART_MAIN | LV_STATE_DEFAULT);

    // Force the first update
    ui_controls_update(true);
}

void ui_controls_init() {
    ui_controls_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_controls_screen, lv_color_hex(0xFAFAFA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_controls_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(
        ui_controls_screen,
        [](lv_event_t *e) -> void {
            lv_event_code_t event_code = lv_event_get_code(e);
            switch (event_code) {
            case LV_EVENT_SCREEN_LOAD_START:
                ui_controls_init_screen_content();
                break;
            case LV_EVENT_SCREEN_UNLOADED:
                lv_obj_clean(ui_controls_screen);
                break;
            }
        },
        LV_EVENT_ALL,
        NULL);
}

void ui_controls_update(bool initialize) {
    if (!initialize && (lv_scr_act() != ui_controls_screen)) {
        return;
    }

    static unsigned long last_update = 0;
    unsigned long current_time = millis();
    if (last_update == 0) {
        last_update = current_time;
    }

    unsigned long delta_time = current_time - last_update;

    if (initialize || (delta_time > CONTROLS_STATE_UPDATE_INTERVAL)) {
        // Retrieve relays state from the queue object
        const bool laser_enabled = digitalRead(PIN_RELAY_LASER) == RELAY_PIN_STATE_ENABLED;
        const bool air_assist_enabled = digitalRead(PIN_RELAY_AIR_ASSIST) == RELAY_PIN_STATE_ENABLED;
        const bool cooling_enabled = digitalRead(PIN_RELAY_COOLING) == RELAY_PIN_STATE_ENABLED;
        const bool lights_enabled = digitalRead(PIN_RELAY_LIGHTS) == RELAY_PIN_STATE_ENABLED;
        const bool beam_preview_enabled = digitalRead(PIN_RELAY_BEAM_PREVIEW) == RELAY_PIN_STATE_ENABLED;

        // Update switches to match the current state
        laser_enabled ? lv_obj_add_state(ui_controls_laser_switch, LV_STATE_CHECKED)
                      : lv_obj_clear_state(ui_controls_laser_switch, LV_STATE_CHECKED);

        air_assist_enabled ? lv_obj_add_state(ui_controls_air_assist_switch, LV_STATE_CHECKED)
                           : lv_obj_clear_state(ui_controls_air_assist_switch, LV_STATE_CHECKED);

        cooling_enabled ? lv_obj_add_state(ui_controls_cooling_switch, LV_STATE_CHECKED)
                        : lv_obj_clear_state(ui_controls_cooling_switch, LV_STATE_CHECKED);

        lights_enabled ? lv_obj_add_state(ui_controls_lights_switch, LV_STATE_CHECKED)
                       : lv_obj_clear_state(ui_controls_lights_switch, LV_STATE_CHECKED);

        beam_preview_enabled ? lv_obj_add_state(ui_controls_preview_switch, LV_STATE_CHECKED)
                             : lv_obj_clear_state(ui_controls_preview_switch, LV_STATE_CHECKED);

        // Reset timer
        last_update = current_time;
    }
}
