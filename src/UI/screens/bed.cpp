#include <Arduino.h>
#include <lvgl.h>
#include <math.h>

#include "K40/bed.h"
#include "UI/images.h"
#include "UI/keyboard.h"
#include "UI/utils.h"
#include "UI/screens/bed.h"
#include "macros.h"
#include "queues.h"

lv_obj_t *ui_bed_screen;

static lv_obj_t *ui_bed_textarea;
static lv_obj_t *ui_bed_home_button;
static lv_obj_t *ui_bed_go_down_button;
static lv_obj_t *ui_bed_go_up_button;
static lv_obj_t *ui_bed_set_origin_button;
static lv_obj_t *ui_bed_focus_surface_button;
static lv_obj_t *ui_bed_focus_center_button;
static lv_obj_t *ui_bed_stop_button;
static lv_obj_t *ui_bed_current_position_value;
static lv_obj_t *ui_bed_target_position_value;
static lv_obj_t *ui_bed_status_image;

static void ui_bed_button_handler(lv_event_t *e) {
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code != LV_EVENT_CLICKED) {
        return;
    }

    lv_obj_t *event_target = lv_event_get_target(e);

    if (event_target == NULL) {
        // Should never happen
        return;
    } else if (event_target == ui_bed_home_button) {
        const BedCommand bed_command = {.type = BED_COMMAND_HOME};
        xQueueOverwrite(bed_command_queue, &bed_command);
    } else if (event_target == ui_bed_set_origin_button) {
        const BedCommand bed_command = {.type = BED_COMMAND_SET_CURRENT_POSITION_AS_ORIGIN};
        xQueueOverwrite(bed_command_queue, &bed_command);
    } else if (event_target == ui_bed_stop_button) {
        const BedCommand bed_command = {.type = BED_COMMAND_STOP};
        xQueueOverwrite(bed_command_queue, &bed_command);
    } else if (event_target == ui_bed_go_down_button || event_target == ui_bed_go_up_button) {
        const BedCommand bed_command = {
            .type = BED_COMMAND_MOVE_RELATIVE,
            .value_nm = (event_target == ui_bed_go_down_button ? -1 : 1) *
                        static_cast<int32_t>(1000000 * atof(lv_textarea_get_text(ui_bed_textarea))),
        };
        xQueueOverwrite(bed_command_queue, &bed_command);
    } else if (event_target == ui_bed_focus_surface_button || event_target == ui_bed_focus_center_button) {
        const BedCommand bed_command = {
            .type = BED_COMMAND_MOVE_ABSOLUTE,
            .value_nm = static_cast<int32_t>(
                1000000 * (event_target == ui_bed_focus_surface_button ? -1 : -1.5) *
                atof(lv_textarea_get_text(ui_bed_textarea))),
        };
        xQueueOverwrite(bed_command_queue, &bed_command);
    }
}

static void ui_bed_init_screen_content() {
    // Make sure the screen is empty
    lv_obj_clean(ui_bed_screen);

    lv_obj_t *ui_bed_main_panel = ui_utils_create_screen_panel(ui_bed_screen);
    lv_obj_t *ui_bed_main_card =
        ui_utils_create_card(ui_bed_main_panel, "BED CONTROLS", LV_SYMBOL_ARROW_DOWN_UP_ACROSS_LINE);
    lv_obj_set_width(ui_bed_main_card, 460);
    lv_obj_set_height(ui_bed_main_card, 255);
    lv_obj_set_pos(ui_bed_main_card, 0, 0);

    ui_bed_textarea = lv_textarea_create(ui_bed_main_card);
    lv_obj_set_width(ui_bed_textarea, 120);
    lv_obj_set_height(ui_bed_textarea, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_bed_textarea, 0);
    lv_obj_set_y(ui_bed_textarea, 69);
    lv_obj_set_style_pad_all(ui_bed_textarea, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_textarea_set_accepted_chars(ui_bed_textarea, "0123456789.");
    lv_textarea_set_placeholder_text(ui_bed_textarea, "Value (mm)");
    lv_textarea_set_one_line(ui_bed_textarea, true);

    ui_bed_home_button = ui_utils_create_small_button(ui_bed_main_card, LV_SYMBOL_HOUSE " Home", 120);
    lv_obj_set_pos(ui_bed_home_button, 0, 35);
    lv_obj_add_event_cb(ui_bed_home_button, ui_bed_button_handler, LV_EVENT_CLICKED, NULL);

    ui_bed_set_origin_button = ui_utils_create_small_button(ui_bed_main_card, LV_SYMBOL_CROSSHAIRS " Set origin", 130);
    lv_obj_set_pos(ui_bed_set_origin_button, 130, 35);
    lv_obj_add_state(ui_bed_set_origin_button, LV_STATE_DISABLED);
    lv_obj_add_event_cb(ui_bed_set_origin_button, ui_bed_button_handler, LV_EVENT_CLICKED, NULL);

    ui_bed_go_up_button = ui_utils_create_small_button(ui_bed_main_card, LV_SYMBOL_CARET_UP " Up", 62);
    lv_obj_set_pos(ui_bed_go_up_button, 130, 70);
    lv_obj_add_event_cb(ui_bed_go_up_button, ui_bed_button_handler, LV_EVENT_CLICKED, NULL);

    ui_bed_go_down_button = ui_utils_create_small_button(ui_bed_main_card, LV_SYMBOL_CARET_DOWN " Down", 62);
    lv_obj_set_pos(ui_bed_go_down_button, 198, 70);
    lv_obj_add_event_cb(ui_bed_go_down_button, ui_bed_button_handler, LV_EVENT_CLICKED, NULL);

    ui_bed_focus_surface_button =
        ui_utils_create_small_button(ui_bed_main_card, LV_SYMBOL_ARROWS_TO_DOT " Focus surface", 170);
    lv_obj_set_pos(ui_bed_focus_surface_button, 270, 35);
    lv_obj_add_state(ui_bed_focus_surface_button, LV_STATE_DISABLED);
    lv_obj_add_event_cb(ui_bed_focus_surface_button, ui_bed_button_handler, LV_EVENT_CLICKED, NULL);

    ui_bed_focus_center_button =
        ui_utils_create_small_button(ui_bed_main_card, LV_SYMBOL_ARROWS_TO_DOT " Focus center", 170);
    lv_obj_set_pos(ui_bed_focus_center_button, 270, 70);
    lv_obj_add_state(ui_bed_focus_center_button, LV_STATE_DISABLED);
    lv_obj_add_event_cb(ui_bed_focus_center_button, ui_bed_button_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *ui_bed_status_panel = lv_obj_create(ui_bed_main_card);
    lv_obj_set_width(ui_bed_status_panel, 190);
    lv_obj_set_height(ui_bed_status_panel, 122);
    lv_obj_set_align(ui_bed_status_panel, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_set_style_bg_opa(ui_bed_status_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_bed_status_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_bed_status_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_bed_status_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_bed_status_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_bed_status_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_bed_stop_button = lv_btn_create(ui_bed_status_panel);
    lv_obj_set_width(ui_bed_stop_button, 50);
    lv_obj_set_height(ui_bed_stop_button, 50);
    lv_obj_set_align(ui_bed_stop_button, LV_ALIGN_TOP_RIGHT);
    lv_obj_add_state(ui_bed_stop_button, LV_STATE_DISABLED);
    lv_obj_set_style_bg_color(ui_bed_stop_button, lv_color_hex(0xE80C0C), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_bed_stop_button, ui_bed_button_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *ui_bed_stop_button_label = lv_label_create(ui_bed_stop_button);
    lv_obj_set_width(ui_bed_stop_button_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_bed_stop_button_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_bed_stop_button_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_bed_stop_button_label, "Stop");

    lv_obj_t *ui_bed_current_position_label = lv_label_create(ui_bed_status_panel);
    lv_obj_set_width(ui_bed_current_position_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_bed_current_position_label, LV_SIZE_CONTENT);
    lv_label_set_text(ui_bed_current_position_label, "Current:");
    lv_obj_set_style_text_font(ui_bed_current_position_label, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_bed_current_position_value = lv_label_create(ui_bed_status_panel);
    lv_obj_set_width(ui_bed_current_position_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_bed_current_position_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_bed_current_position_value, 60);
    lv_obj_set_y(ui_bed_current_position_value, 0);
    lv_label_set_text(ui_bed_current_position_value, "-");
    lv_obj_set_style_text_color(ui_bed_current_position_value, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_bed_current_position_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_bed_target_position_label = lv_label_create(ui_bed_status_panel);
    lv_obj_set_width(ui_bed_target_position_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_bed_target_position_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_bed_target_position_label, 0);
    lv_obj_set_y(ui_bed_target_position_label, 18);
    lv_label_set_text(ui_bed_target_position_label, "Target:");
    lv_obj_set_style_text_font(ui_bed_target_position_label, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_bed_target_position_value = lv_label_create(ui_bed_status_panel);
    lv_obj_set_width(ui_bed_target_position_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_bed_target_position_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_bed_target_position_value, 60);
    lv_obj_set_y(ui_bed_target_position_value, 18);
    lv_label_set_text(ui_bed_target_position_value, "-");
    lv_obj_set_style_text_color(ui_bed_target_position_value, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_bed_target_position_value, &font_default_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_bed_image = lv_img_create(ui_bed_status_panel);
    lv_img_set_src(ui_bed_image, &image_bed);
    lv_obj_set_width(ui_bed_image, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_bed_image, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_bed_image, LV_ALIGN_BOTTOM_LEFT);

    ui_bed_status_image = lv_img_create(ui_bed_image);
    lv_img_set_src(ui_bed_status_image, &anim_up_1);
    lv_obj_set_width(ui_bed_status_image, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_bed_status_image, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_bed_status_image, LV_ALIGN_CENTER);
    lv_obj_set_y(ui_bed_status_image, 2);
    lv_obj_add_flag(ui_bed_status_image, LV_OBJ_FLAG_HIDDEN);
    lv_img_set_zoom(ui_bed_status_image, 128);

    lv_obj_t *ui_bed_keyboard = lv_keyboard_create(ui_bed_main_card);
    lv_obj_set_width(ui_bed_keyboard, 210);
    lv_obj_set_height(ui_bed_keyboard, 120);
    lv_obj_set_align(ui_bed_keyboard, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_bg_opa(ui_bed_keyboard, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_bed_keyboard, &font_default_14, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_bed_keyboard, lv_palette_main(LV_PALETTE_BLUE), LV_PART_ITEMS);
    lv_obj_set_style_text_color(ui_bed_keyboard, lv_color_white(), LV_PART_ITEMS);
    lv_keyboard_set_map(ui_bed_keyboard, LV_KEYBOARD_MODE_USER_1, KEYBOARD_NUMPAD_MAP, KEYBOARD_NUMPAD_CTRL);
    lv_keyboard_set_mode(ui_bed_keyboard, LV_KEYBOARD_MODE_USER_1);
    lv_keyboard_set_textarea(ui_bed_keyboard, ui_bed_textarea);

    // Force the first update
    ui_bed_update(true);
}

void ui_bed_init() {
    ui_bed_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(ui_bed_screen, lv_color_hex(0xFAFAFA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_bed_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(
        ui_bed_screen,
        [](lv_event_t *e) -> void {
            lv_event_code_t event_code = lv_event_get_code(e);
            switch (event_code) {
            case LV_EVENT_SCREEN_LOAD_START:
                ui_bed_init_screen_content();
                break;
            case LV_EVENT_SCREEN_UNLOADED:
                lv_obj_clean(ui_bed_screen);
                break;
            }
        },
        LV_EVENT_ALL,
        NULL);
}

void ui_bed_update(bool initialize) {
    if (!initialize && (lv_scr_act() != ui_bed_screen)) {
        return;
    }

    static unsigned long last_update = 0;
    unsigned long current_time = millis();
    if (last_update == 0) {
        last_update = current_time;
    }

    unsigned long delta_time = current_time - last_update;

    if (initialize || (delta_time > BED_STATE_UPDATE_INTERVAL)) {
        static BedStatus current_bed_status;

        // Retrieve bed state from the queue object
        if (xQueuePeek(bed_current_status_queue, &current_bed_status, 0) == pdTRUE) {
            BedState current_bed_state = current_bed_status.state;

            if (current_bed_state == BED_STATE_IDLE) {
                // Hide the status image if the bed is idling
                lv_obj_add_flag(ui_bed_status_image, LV_OBJ_FLAG_HIDDEN);
            } else {
                // Chose the frame based on the current time
                // All animations should be 3 frames long
                uint32_t current_frame = (current_time / 200) % 3;

                // Show the status image if the bed is moving
                lv_obj_clear_flag(ui_bed_status_image, LV_OBJ_FLAG_HIDDEN);

                // Rotate the image if the bed is going down
                if (current_bed_state == BED_STATE_GOING_DOWN) {
                    lv_img_set_angle(ui_bed_status_image, 1800);
                } else {
                    lv_img_set_angle(ui_bed_status_image, 0);
                }

                // Set the status image
                switch (current_bed_state) {
                case BED_STATE_GOING_DOWN:
                case BED_STATE_GOING_UP:
                    switch (current_frame) {
                    case 0:
                        lv_img_set_src(ui_bed_status_image, &anim_up_1);
                        break;
                    case 1:
                        lv_img_set_src(ui_bed_status_image, &anim_up_2);
                        break;
                    case 2:
                        lv_img_set_src(ui_bed_status_image, &anim_up_3);
                        break;
                    }
                    break;
                case BED_STATE_HOMING:
                    switch (current_frame) {
                    case 0:
                        lv_img_set_src(ui_bed_status_image, &anim_home_1);
                        break;
                    case 1:
                        lv_img_set_src(ui_bed_status_image, &anim_home_2);
                        break;
                    case 2:
                        lv_img_set_src(ui_bed_status_image, &anim_home_3);
                        break;
                    }
                    break;
                }
            }

            // Update current position
            if (current_bed_status.current.is_set) {
                int32_t current_pos_nm = current_bed_status.current.position_nm;
                if (current_bed_status.origin.is_set) {
                    current_pos_nm -= current_bed_status.origin.position_nm;
                }

                char formatted_current_position[10];
                snprintf(
                    formatted_current_position,
                    ARRAY_SIZE(formatted_current_position),
                    "%.2fmm",
                    current_pos_nm / 1000000.f);
                lv_label_set_text(ui_bed_current_position_value, formatted_current_position);
            } else {
                lv_label_set_text(ui_bed_current_position_value, "-");
            }

            // Update target position
            if (current_bed_status.current.is_set && current_bed_status.origin.is_set &&
                current_bed_status.target.is_set) {
                int32_t target_pos_nm = current_bed_status.target.position_nm;
                if (current_bed_status.origin.is_set) {
                    target_pos_nm -= current_bed_status.origin.position_nm;
                }

                char formatted_target_position[10];
                snprintf(
                    formatted_target_position,
                    ARRAY_SIZE(formatted_target_position),
                    "%.2fmm",
                    target_pos_nm / 1000000.f);
                lv_label_set_text(ui_bed_target_position_value, formatted_target_position);
            } else {
                lv_label_set_text(ui_bed_target_position_value, "-");
            }

            // Enable/disable absolute move/set origin buttons
            if (current_bed_status.current.is_set) {
                lv_obj_clear_state(ui_bed_focus_surface_button, LV_STATE_DISABLED);
                lv_obj_clear_state(ui_bed_focus_center_button, LV_STATE_DISABLED);
                lv_obj_clear_state(ui_bed_set_origin_button, LV_STATE_DISABLED);
            } else {
                lv_obj_add_state(ui_bed_focus_surface_button, LV_STATE_DISABLED);
                lv_obj_add_state(ui_bed_focus_center_button, LV_STATE_DISABLED);
                lv_obj_add_state(ui_bed_set_origin_button, LV_STATE_DISABLED);
            }

            // Enable/disable stop button
            if (current_bed_status.state == BED_STATE_IDLE) {
                lv_obj_add_state(ui_bed_stop_button, LV_STATE_DISABLED);
            } else {
                lv_obj_clear_state(ui_bed_stop_button, LV_STATE_DISABLED);
            }
        }

        // Reset timer
        last_update = current_time;
    }
}
