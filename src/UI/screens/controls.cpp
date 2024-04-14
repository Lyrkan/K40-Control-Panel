#include <Arduino.h>
#include <lvgl.h>

#include "Grbl/grbl_serial.h"
#include "Grbl/grbl_report.h"
#include "K40/relays.h"
#include "UI/images.h"
#include "UI/overlay.h"
#include "UI/utils.h"
#include "UI/screens/controls.h"
#include "math.h"
#include "macros.h"
#include "mutex.h"
#include "queues.h"

lv_obj_t *ui_controls_screen;

static StaticEventGroup_t ui_controls_event_group_static;
static EventGroupHandle_t ui_controls_event_group = xEventGroupCreateStatic(&ui_controls_event_group_static);

static lv_obj_t *ui_controls_laser_speed_textarea;
static lv_obj_t *ui_controls_laser_pos_x_textarea;
static lv_obj_t *ui_controls_laser_pos_y_textarea;
static lv_obj_t *ui_controls_laser_home_button;
static lv_obj_t *ui_controls_laser_disable_steppers_button;
static lv_obj_t *ui_controls_laser_move_x_matrix;
static lv_obj_t *ui_controls_laser_move_y_matrix;
static lv_obj_t *ui_controls_interlock_switch;
static lv_obj_t *ui_controls_air_assist_switch;
static lv_obj_t *ui_controls_lights_switch;
static lv_obj_t *ui_controls_preview_switch;

static void ui_controls_lock_grbl_controls() {
    lv_obj_add_state(ui_controls_laser_home_button, LV_STATE_DISABLED);
    lv_obj_add_state(ui_controls_laser_disable_steppers_button, LV_STATE_DISABLED);
    lv_btnmatrix_set_btn_ctrl_all(ui_controls_laser_move_x_matrix, LV_BTNMATRIX_CTRL_DISABLED);
    lv_btnmatrix_set_btn_ctrl_all(ui_controls_laser_move_y_matrix, LV_BTNMATRIX_CTRL_DISABLED);
}

static void ui_controls_unlock_grbl_controls() {
    lv_obj_clear_state(ui_controls_laser_home_button, LV_STATE_DISABLED);
    lv_obj_clear_state(ui_controls_laser_disable_steppers_button, LV_STATE_DISABLED);
    lv_btnmatrix_clear_btn_ctrl_all(ui_controls_laser_move_x_matrix, LV_BTNMATRIX_CTRL_DISABLED);
    lv_btnmatrix_clear_btn_ctrl_all(ui_controls_laser_move_y_matrix, LV_BTNMATRIX_CTRL_DISABLED);
}

static void ui_controls_btnmatrix_handler(lv_event_t *e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code != LV_EVENT_VALUE_CHANGED) {
        return;
    }

    lv_obj_t *event_target = lv_event_get_target(e);

    // Detect when GRBL command are fully process to release UI
    GrblCommandCallbacks grbl_command_callbacks = GrblCommandCallbacks();
    grbl_command_callbacks.on_finished = []() -> void {
        ui_controls_notify_update(CONTROLS_UPDATE_GRBL_COMMMAND_ENDED);
    };

    if (event_target == NULL) {
        // Should never happen
        return;
    } else if (event_target == ui_controls_laser_move_x_matrix || event_target == ui_controls_laser_move_y_matrix) {
        uint32_t btn_id = lv_btnmatrix_get_selected_btn(event_target);
        if (btn_id == 3) { // Home
            grbl_command_callbacks.on_failure = []() -> void {
                ui_overlay_add_flash_message(FLASH_LEVEL_DANGER, "Homing failed or timed out");
            };

            if (event_target == ui_controls_laser_move_x_matrix) {
                grbl_command_callbacks.on_success = []() -> void {
                    ui_overlay_add_flash_message(FLASH_LEVEL_SUCCESS, "X axis homed");
                };
                ui_controls_lock_grbl_controls();
                grbl_send_home_command(GRBL_AXIS_X, grbl_command_callbacks);
            } else if (event_target == ui_controls_laser_move_y_matrix) {
                grbl_command_callbacks.on_success = []() -> void {
                    ui_overlay_add_flash_message(FLASH_LEVEL_SUCCESS, "Y axis homed");
                };
                ui_controls_lock_grbl_controls();
                grbl_send_home_command(GRBL_AXIS_Y, grbl_command_callbacks);
            }
        } else { // Relative move
            float_t move_offset = pow10(abs((int)(btn_id - 3)) - 1) * (btn_id < 3 ? -1 : 1);
            GrblMoveCoordinates move_target = {.axis_flags = 0};
            if (event_target == ui_controls_laser_move_x_matrix) {
                move_target.axis_flags = GRBL_AXIS_X;
                move_target.x = move_offset;
            } else if (event_target == ui_controls_laser_move_y_matrix) {
                move_target.axis_flags = GRBL_AXIS_Y;
                move_target.y = move_offset;
            }

            grbl_command_callbacks.on_failure = []() -> void {
                ui_overlay_add_flash_message(FLASH_LEVEL_DANGER, "Move command failed or timed out");
            };

            ui_controls_lock_grbl_controls();
            grbl_send_move_command(move_target, GRBL_MOVE_MODE_RELATIVE, grbl_command_callbacks);
        }
    }
}

static void ui_controls_button_handler(lv_event_t *e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code != LV_EVENT_CLICKED) {
        return;
    }

    lv_obj_t *event_target = lv_event_get_target(e);

    // Detect when GRBL command are fully process to release UI
    GrblCommandCallbacks grbl_command_callbacks = GrblCommandCallbacks();
    grbl_command_callbacks.on_finished = []() -> void {
        ui_controls_notify_update(CONTROLS_UPDATE_GRBL_COMMMAND_ENDED);
    };

    if (event_target == NULL) {
        // Should never happen
        return;
    } else if (event_target == ui_controls_laser_home_button) {
        grbl_command_callbacks.on_failure = []() -> void {
            ui_overlay_add_flash_message(FLASH_LEVEL_DANGER, "Homing failed or timed out");
        };

        grbl_command_callbacks.on_success = []() -> void {
            ui_overlay_add_flash_message(FLASH_LEVEL_SUCCESS, "Homing done");
        };

        ui_controls_lock_grbl_controls();
        grbl_send_home_command(GRBL_AXIS_X | GRBL_AXIS_Y, grbl_command_callbacks);
    } else if (event_target == ui_controls_laser_disable_steppers_button) {
        grbl_command_callbacks.on_failure = []() -> void {
            ui_overlay_add_flash_message(FLASH_LEVEL_DANGER, "Could not disable steppers");
        };

        grbl_command_callbacks.on_success = []() -> void {
            ui_overlay_add_flash_message(FLASH_LEVEL_SUCCESS, "Steppers are now disabled");
        };

        ui_controls_lock_grbl_controls();
        grbl_send_message("$MD", false, GRBL_ACK_DEFAULT_TIMEOUT_MS, grbl_command_callbacks);
    }
}

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

    ui_controls_laser_home_button = ui_utils_create_small_button(ui_controls_laser_card, LV_SYMBOL_HOUSE " Home", 80);
    lv_obj_set_pos(ui_controls_laser_home_button, 100, 25);
    lv_obj_add_event_cb(ui_controls_laser_home_button, ui_controls_button_handler, LV_EVENT_CLICKED, NULL);

    ui_controls_laser_disable_steppers_button =
        ui_utils_create_small_button(ui_controls_laser_card, LV_SYMBOL_UNLOCK " Disable steppers", 130);
    lv_obj_set_pos(ui_controls_laser_disable_steppers_button, 185, 25);
    lv_obj_add_event_cb(ui_controls_laser_disable_steppers_button, ui_controls_button_handler, LV_EVENT_CLICKED, NULL);

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
    ui_controls_laser_move_x_matrix = lv_btnmatrix_create(ui_controls_laser_card);
    lv_btnmatrix_set_map(ui_controls_laser_move_x_matrix, laser_head_moves_map);
    lv_obj_add_style(ui_controls_laser_move_x_matrix, &laser_head_moves_matrix_style_bg, 0);
    lv_obj_add_style(ui_controls_laser_move_x_matrix, &laser_head_moves_matrix_style_btn, LV_PART_ITEMS);
    lv_obj_set_size(ui_controls_laser_move_x_matrix, 340, 25);
    lv_obj_set_pos(ui_controls_laser_move_x_matrix, 100, 60);
    lv_obj_add_event_cb(ui_controls_laser_move_x_matrix, ui_controls_btnmatrix_handler, LV_EVENT_VALUE_CHANGED, NULL);

    ui_controls_laser_move_y_matrix = lv_btnmatrix_create(ui_controls_laser_card);
    lv_btnmatrix_set_map(ui_controls_laser_move_y_matrix, laser_head_moves_map);
    lv_obj_add_style(ui_controls_laser_move_y_matrix, &laser_head_moves_matrix_style_bg, 0);
    lv_obj_add_style(ui_controls_laser_move_y_matrix, &laser_head_moves_matrix_style_btn, LV_PART_ITEMS);
    lv_obj_set_size(ui_controls_laser_move_y_matrix, 340, 25);
    lv_obj_set_pos(ui_controls_laser_move_y_matrix, 100, 95);
    lv_obj_add_event_cb(ui_controls_laser_move_y_matrix, ui_controls_btnmatrix_handler, LV_EVENT_VALUE_CHANGED, NULL);

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

    // Update fields that depend on the last Grbl report when a change is detected
    static char laser_pos_x_formatted_value[10];
    static char laser_pos_y_formatted_value[10];
    static char laser_speed_formatted_value[10];

    uint8_t pending_updates = xEventGroupGetBits(ui_controls_event_group);
    bool pending_grbl_report_update = initialize || ((pending_updates & CONTROLS_UPDATE_GRBL_REPORT) != 0);
    bool grbl_command_ended_update = initialize || ((pending_updates & CONTROLS_UPDATE_GRBL_COMMMAND_ENDED) != 0);

    if (pending_grbl_report_update) {
        TAKE_MUTEX(grbl_last_report_mutex)
        GrblCoord m_pos = grbl_last_report.m_pos;
        GrblFeedState feed = grbl_last_report.feed_state;
        RELEASE_MUTEX(grbl_last_report_mutex)

        if (m_pos.is_set) {
            snprintf(laser_pos_x_formatted_value, ARRAY_SIZE(laser_pos_x_formatted_value), "%.2f", m_pos.x);
            snprintf(laser_pos_y_formatted_value, ARRAY_SIZE(laser_pos_y_formatted_value), "%.2f", m_pos.y);
            lv_textarea_set_text(ui_controls_laser_pos_x_textarea, laser_pos_x_formatted_value);
            lv_textarea_set_text(ui_controls_laser_pos_y_textarea, laser_pos_y_formatted_value);
        }

        if (feed.is_set) {
            snprintf(laser_speed_formatted_value, ARRAY_SIZE(laser_speed_formatted_value), "%d", feed.rate);
            lv_textarea_set_text(ui_controls_laser_speed_textarea, laser_speed_formatted_value);
        }

        xEventGroupClearBits(ui_controls_event_group, CONTROLS_UPDATE_GRBL_REPORT);
    }

    if (grbl_command_ended_update) {
        ui_controls_unlock_grbl_controls();
        xEventGroupClearBits(ui_controls_event_group, CONTROLS_UPDATE_GRBL_COMMMAND_ENDED);
    }

    // Update relays state on an interval
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

void ui_controls_notify_update(uint8_t update_types) { xEventGroupSetBits(ui_controls_event_group, update_types); }
