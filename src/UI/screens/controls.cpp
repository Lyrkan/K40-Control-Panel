#include <Arduino.h>
#include <lvgl.h>

#include "K40/relays.h"
#include "UI/images.h"
#include "UI/utils.h"
#include "UI/screens/controls.h"
#include "queues.h"

lv_obj_t *ui_controls_screen;

static lv_obj_t *ui_controls_laser_speed_textarea;
static lv_obj_t *ui_controls_laser_pos_x_textarea;
static lv_obj_t *ui_controls_laser_pos_y_textarea;
static lv_obj_t *ui_controls_interlock_switch;
static lv_obj_t *ui_controls_air_assist_switch;
static lv_obj_t *ui_controls_lights_switch;
static lv_obj_t *ui_controls_preview_switch;

static void ui_controls_switch_handler(lv_event_t *e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code != LV_EVENT_VALUE_CHANGED) {
        return;
    }

    lv_obj_t *target = lv_event_get_target(e);

    int32_t relay_pin = -1;
    if (target == ui_controls_interlock_switch) {
        relay_pin = RELAY_PIN_INTERLOCK;
    } else if (target == ui_controls_air_assist_switch) {
        relay_pin = RELAY_PIN_AIR_ASSIST;
    } else if (target == ui_controls_lights_switch) {
        relay_pin = RELAY_PIN_LIGHTS;
    } else if (target == ui_controls_preview_switch) {
        relay_pin = RELAY_PIN_BEAM_PREVIEW;
    }

    if (relay_pin != -1) {
        RelaysCommand command = {
            .pin = (RelayPin)relay_pin,
            .state = lv_obj_has_state(target, LV_STATE_CHECKED) ? RELAY_STATE_ENABLED : RELAY_STATE_DISABLED,
        };

        xQueueSendToBack(relays_command_queue, &command, pdMS_TO_TICKS(100));
    }
}

void ui_controls_init_screen_content() {
    // Make sure the screen is empty
    lv_obj_clean(ui_controls_screen);

    lv_obj_t *ui_controls_main_panel = ui_utils_create_screen_panel(ui_controls_screen);

    // Laser card
    lv_obj_t *ui_controls_laser_card =
        ui_utils_create_card(ui_controls_main_panel, "LASER HEAD", LV_SYMBOL_UP_DOWN_LEFT_RIGHT);
    lv_obj_set_width(ui_controls_laser_card, 460);
    lv_obj_set_height(ui_controls_laser_card, 140);
    lv_obj_set_pos(ui_controls_laser_card, 0, 0);

    lv_obj_t *ui_controls_laser_speed_label = lv_label_create(ui_controls_laser_card);
    lv_obj_set_width(ui_controls_laser_speed_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_laser_speed_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_laser_speed_label, 0);
    lv_obj_set_y(ui_controls_laser_speed_label, 28);
    lv_label_set_text(ui_controls_laser_speed_label, "Speed:");

    ui_controls_laser_speed_textarea = lv_textarea_create(ui_controls_laser_card);
    lv_obj_set_width(ui_controls_laser_speed_textarea, 50);
    lv_obj_set_height(ui_controls_laser_speed_textarea, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_laser_speed_textarea, 45);
    lv_obj_set_y(ui_controls_laser_speed_textarea, 23);
    lv_obj_set_style_pad_all(ui_controls_laser_speed_textarea, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_accepted_chars(ui_controls_laser_speed_textarea, "0123456789.");
    lv_textarea_set_placeholder_text(ui_controls_laser_speed_textarea, "mm/s");
    lv_textarea_set_one_line(ui_controls_laser_speed_textarea, true);

    lv_obj_t *ui_controls_laser_home_button =
        ui_utils_create_small_button(ui_controls_laser_card, LV_SYMBOL_HOUSE " Home", 80);
    lv_obj_set_pos(ui_controls_laser_home_button, 100, 25);

    lv_obj_t *ui_controls_laser_disable_steppers_button =
        ui_utils_create_small_button(ui_controls_laser_card, LV_SYMBOL_UNLOCK " Disable steppers", 130);
    lv_obj_set_pos(ui_controls_laser_disable_steppers_button, 185, 25);

    lv_obj_t *ui_controls_laser_pos_x_label = lv_label_create(ui_controls_laser_card);
    lv_obj_set_width(ui_controls_laser_pos_x_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_laser_pos_x_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_laser_pos_x_label, 0);
    lv_obj_set_y(ui_controls_laser_pos_x_label, 63);
    lv_label_set_text(ui_controls_laser_pos_x_label, "X:");

    lv_obj_t *ui_controls_laser_pos_y_label = lv_label_create(ui_controls_laser_card);
    lv_obj_set_width(ui_controls_laser_pos_y_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_laser_pos_y_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_laser_pos_y_label, 0);
    lv_obj_set_y(ui_controls_laser_pos_y_label, 99);
    lv_label_set_text(ui_controls_laser_pos_y_label, "Y:");

    ui_controls_laser_pos_x_textarea = lv_textarea_create(ui_controls_laser_card);
    lv_obj_set_width(ui_controls_laser_pos_x_textarea, 75);
    lv_obj_set_height(ui_controls_laser_pos_x_textarea, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_laser_pos_x_textarea, 20);
    lv_obj_set_y(ui_controls_laser_pos_x_textarea, 58);
    lv_obj_set_style_pad_all(ui_controls_laser_pos_x_textarea, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_accepted_chars(ui_controls_laser_pos_x_textarea, "0123456789.");
    lv_textarea_set_placeholder_text(ui_controls_laser_pos_x_textarea, "Pos (mm)");
    lv_textarea_set_one_line(ui_controls_laser_pos_x_textarea, true);

    ui_controls_laser_pos_y_textarea = lv_textarea_create(ui_controls_laser_card);
    lv_obj_set_width(ui_controls_laser_pos_y_textarea, 75);
    lv_obj_set_height(ui_controls_laser_pos_y_textarea, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_laser_pos_y_textarea, 20);
    lv_obj_set_y(ui_controls_laser_pos_y_textarea, 93);
    lv_obj_set_style_pad_all(ui_controls_laser_pos_y_textarea, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_accepted_chars(ui_controls_laser_pos_y_textarea, "0123456789.");
    lv_textarea_set_placeholder_text(ui_controls_laser_pos_y_textarea, "Pos (mm)");
    lv_textarea_set_one_line(ui_controls_laser_pos_y_textarea, true);

    static lv_style_t laser_head_moves_matrix_style_bg;
    lv_style_init(&laser_head_moves_matrix_style_bg);
    lv_style_set_pad_all(&laser_head_moves_matrix_style_bg, 0);
    lv_style_set_pad_gap(&laser_head_moves_matrix_style_bg, 0);
    lv_style_set_clip_corner(&laser_head_moves_matrix_style_bg, true);
    lv_style_set_radius(&laser_head_moves_matrix_style_bg, 6);
    lv_style_set_border_width(&laser_head_moves_matrix_style_bg, 0);

    static lv_style_t laser_head_moves_matrix_style_btn;
    lv_style_init(&laser_head_moves_matrix_style_btn);
    lv_style_set_radius(&laser_head_moves_matrix_style_btn, 0);
    lv_style_set_border_width(&laser_head_moves_matrix_style_btn, 1);
    lv_style_set_border_opa(&laser_head_moves_matrix_style_btn, LV_OPA_50);
    lv_style_set_border_color(&laser_head_moves_matrix_style_btn, lv_color_white());
    lv_style_set_border_side(&laser_head_moves_matrix_style_btn, LV_BORDER_SIDE_INTERNAL);
    lv_style_set_bg_color(&laser_head_moves_matrix_style_btn, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_text_color(&laser_head_moves_matrix_style_btn, lv_color_white());
    lv_style_set_radius(&laser_head_moves_matrix_style_btn, 0);

    static const char *laser_head_moves_map[] = {"-100", "-10", "-1", LV_SYMBOL_HOUSE, "+1", "+10", "+100", ""};
    lv_obj_t *ui_controls_laser_move_x_matrix = lv_btnmatrix_create(ui_controls_laser_card);
    lv_btnmatrix_set_map(ui_controls_laser_move_x_matrix, laser_head_moves_map);
    lv_obj_add_style(ui_controls_laser_move_x_matrix, &laser_head_moves_matrix_style_bg, 0);
    lv_obj_add_style(ui_controls_laser_move_x_matrix, &laser_head_moves_matrix_style_btn, LV_PART_ITEMS);
    lv_obj_set_size(ui_controls_laser_move_x_matrix, 340, 25);
    lv_obj_set_pos(ui_controls_laser_move_x_matrix, 100, 60);

    lv_obj_t *ui_controls_laser_move_y_matrix = lv_btnmatrix_create(ui_controls_laser_card);
    lv_btnmatrix_set_map(ui_controls_laser_move_y_matrix, laser_head_moves_map);
    lv_obj_add_style(ui_controls_laser_move_y_matrix, &laser_head_moves_matrix_style_bg, 0);
    lv_obj_add_style(ui_controls_laser_move_y_matrix, &laser_head_moves_matrix_style_btn, LV_PART_ITEMS);
    lv_obj_set_size(ui_controls_laser_move_y_matrix, 340, 25);
    lv_obj_set_pos(ui_controls_laser_move_y_matrix, 100, 95);

    // Toggles card
    lv_obj_t *ui_controls_toggles_card =
        ui_utils_create_card(ui_controls_main_panel, "TOGGLES", LV_SYMBOL_PLUG_CIRCLE_BOLT);
    lv_obj_set_width(ui_controls_toggles_card, 460);
    lv_obj_set_height(ui_controls_toggles_card, 105);
    lv_obj_set_pos(ui_controls_toggles_card, 0, 150);

    lv_obj_t *ui_controls_interlock_switch_label = lv_label_create(ui_controls_toggles_card);
    lv_obj_set_width(ui_controls_interlock_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_interlock_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_interlock_switch_label, 0);
    lv_obj_set_y(ui_controls_interlock_switch_label, 30);
    lv_label_set_text(ui_controls_interlock_switch_label, "Laser interlock");

    lv_obj_t *ui_controls_air_assist_switch_label = lv_label_create(ui_controls_toggles_card);
    lv_obj_set_width(ui_controls_air_assist_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_air_assist_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_air_assist_switch_label, 0);
    lv_obj_set_y(ui_controls_air_assist_switch_label, 60);
    lv_label_set_text(ui_controls_air_assist_switch_label, "Air Assist");

    lv_obj_t *ui_controls_lights_switch_label = lv_label_create(ui_controls_toggles_card);
    lv_obj_set_width(ui_controls_lights_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_lights_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_lights_switch_label, 230);
    lv_obj_set_y(ui_controls_lights_switch_label, 30);
    lv_label_set_text(ui_controls_lights_switch_label, "Lights");

    lv_obj_t *ui_controls_preview_switch_label = lv_label_create(ui_controls_toggles_card);
    lv_obj_set_width(ui_controls_preview_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_controls_preview_switch_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_controls_preview_switch_label, 230);
    lv_obj_set_y(ui_controls_preview_switch_label, 60);
    lv_label_set_text(ui_controls_preview_switch_label, "Beam preview");

    ui_controls_interlock_switch = lv_switch_create(ui_controls_toggles_card);
    lv_obj_set_width(ui_controls_interlock_switch, 50);
    lv_obj_set_height(ui_controls_interlock_switch, 25);
    lv_obj_set_x(ui_controls_interlock_switch, 100);
    lv_obj_set_y(ui_controls_interlock_switch, 30);
    lv_obj_add_event_cb(ui_controls_interlock_switch, ui_controls_switch_handler, LV_EVENT_VALUE_CHANGED, NULL);

    ui_controls_air_assist_switch = lv_switch_create(ui_controls_toggles_card);
    lv_obj_set_width(ui_controls_air_assist_switch, 50);
    lv_obj_set_height(ui_controls_air_assist_switch, 25);
    lv_obj_set_x(ui_controls_air_assist_switch, 100);
    lv_obj_set_y(ui_controls_air_assist_switch, 60);
    lv_obj_add_event_cb(ui_controls_air_assist_switch, ui_controls_switch_handler, LV_EVENT_VALUE_CHANGED, NULL);

    ui_controls_lights_switch = lv_switch_create(ui_controls_toggles_card);
    lv_obj_set_width(ui_controls_lights_switch, 50);
    lv_obj_set_height(ui_controls_lights_switch, 25);
    lv_obj_set_x(ui_controls_lights_switch, 330);
    lv_obj_set_y(ui_controls_lights_switch, 30);
    lv_obj_add_event_cb(ui_controls_lights_switch, ui_controls_switch_handler, LV_EVENT_VALUE_CHANGED, NULL);

    ui_controls_preview_switch = lv_switch_create(ui_controls_toggles_card);
    lv_obj_set_width(ui_controls_preview_switch, 50);
    lv_obj_set_height(ui_controls_preview_switch, 25);
    lv_obj_set_x(ui_controls_preview_switch, 330);
    lv_obj_set_y(ui_controls_preview_switch, 60);
    lv_obj_add_event_cb(ui_controls_preview_switch, ui_controls_switch_handler, LV_EVENT_VALUE_CHANGED, NULL);

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
        const bool interlock_active = relays_is_active(RELAY_PIN_INTERLOCK);
        const bool air_assist_active = relays_is_active(RELAY_PIN_AIR_ASSIST);
        const bool lights_active = relays_is_active(RELAY_PIN_LIGHTS);
        const bool beam_preview_active = relays_is_active(RELAY_PIN_BEAM_PREVIEW);

        const bool interlock_disabled = relays_is_disabled(RELAY_PIN_INTERLOCK);

        // Update switches to match the current state
        interlock_active ? lv_obj_add_state(ui_controls_interlock_switch, LV_STATE_CHECKED)
                         : lv_obj_clear_state(ui_controls_interlock_switch, LV_STATE_CHECKED);

        interlock_disabled ? lv_obj_add_state(ui_controls_interlock_switch, LV_STATE_DISABLED)
                           : lv_obj_clear_state(ui_controls_interlock_switch, LV_STATE_DISABLED);

        air_assist_active ? lv_obj_add_state(ui_controls_air_assist_switch, LV_STATE_CHECKED)
                          : lv_obj_clear_state(ui_controls_air_assist_switch, LV_STATE_CHECKED);

        lights_active ? lv_obj_add_state(ui_controls_lights_switch, LV_STATE_CHECKED)
                      : lv_obj_clear_state(ui_controls_lights_switch, LV_STATE_CHECKED);

        beam_preview_active ? lv_obj_add_state(ui_controls_preview_switch, LV_STATE_CHECKED)
                            : lv_obj_clear_state(ui_controls_preview_switch, LV_STATE_CHECKED);

        // Reset timer
        last_update = current_time;
    }
}
