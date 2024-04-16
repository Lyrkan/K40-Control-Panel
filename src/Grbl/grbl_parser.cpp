#include <Arduino.h>
#include <string.h>
#include <freertos/FreeRTOS.h>

#include "Grbl/grbl_parser.h"
#include "Grbl/grbl_report.h"
#include "Grbl/grbl_serial.h"
#include "Grbl/grbl_state.h"
#include "UI/overlay.h"
#include "tasks.h"

static bool grbl_remove_prefix_suffix(char **input, const char *prefix, const char *suffix = NULL) {
    size_t input_length = strnlen(*input, GRBL_MAX_LINE_lENGTH);
    size_t prefix_length = strnlen(prefix, GRBL_MAX_LINE_lENGTH);

    if (prefix_length > input_length) {
        return false;
    }

    if (strncmp(*input, prefix, prefix_length) != 0) {
        return false;
    }

    if (suffix != NULL) {
        size_t suffix_length = strnlen(suffix, GRBL_MAX_LINE_lENGTH);
        if (suffix_length > input_length - prefix_length) {
            return false;
        }

        if (strncmp(*input + input_length - suffix_length, suffix, suffix_length) != 0) {
            return false;
        }

        (*input)[input_length - suffix_length] = '\0';
    }

    *input = *input + prefix_length;
    return true;
}

static void grbl_process_ack() {
    log_d("Ack received");

    // Notify the TX task that its previous message has been acknowledged
    if (grbl_tx_task_handle != NULL) {
        xTaskNotifyIndexed(
            grbl_tx_task_handle,
            GRBL_TASK_NOTIFY_ACK_INDEX,
            GRBL_TASK_NOTIFY_ACK_SUCCESS,
            eSetValueWithOverwrite);
    }
}

static void grbl_process_error(const char *error_code) {
    int error_code_i = error_code[0] != '\0' ? atoi(error_code) : -1;
    const char *error_description = grbl_error_to_string((GrblError)error_code_i);

    log_e("Error message received (code: %d): %s", error_code_i, error_description);
    ui_overlay_add_flash_message(FLASH_LEVEL_DANGER, error_description);

    // Notify the TX task that its previous message has been acknowledged
    if (grbl_tx_task_handle != NULL) {
        xTaskNotifyIndexed(
            grbl_tx_task_handle,
            GRBL_TASK_NOTIFY_ACK_INDEX,
            GRBL_TASK_NOTIFY_ACK_ERROR,
            eSetValueWithOverwrite);
    }
}

static void grbl_process_alarm(const char *alarm_code) {
    int alarm_code_i = alarm_code[0] != '\0' ? atoi(alarm_code) : -1;
    const char *alarm_description = grbl_alarm_to_string((GrblAlarm)alarm_code_i);

    log_e("Alarm triggered (code: %d): %s", alarm_code_i, alarm_description);
    ui_overlay_add_flash_message(FLASH_LEVEL_DANGER, alarm_description);
    grbl_update_last_alarm((GrblAlarm)alarm_code_i);
}

static void grbl_process_settings(const char *settings_body) { log_d("Setting value: %s", settings_body); }

static GrblCoord grbl_process_position(char *positions) {
    GrblCoord pos = {
        .is_set = false,
        .x = 0,
        .y = 0,
        .z = 0,
    };

    char *pos_token_saveptr;
    int i = 0;
    for (char *pos_token = strtok_r(positions, ",", &pos_token_saveptr); pos_token != NULL;
         pos_token = strtok_r(NULL, ",", &pos_token_saveptr), i++) {
        switch (i) {
        case 0:
            pos.is_set = true;
            pos.x = atof(pos_token);
            break;
        case 1:
            pos.y = atof(pos_token);
            break;
        case 2:
            pos.z = atof(pos_token);
            break;
        }
    }

    return pos;
}

static void grbl_process_report(char *report_body) {
    log_d("Report received: %s", report_body);

    GrblReport report;
    char *report_token_saveptr;
    for (char *report_token = strtok_r(report_body, "|", &report_token_saveptr); report_token != NULL;
         report_token = strtok_r(NULL, "|", &report_token_saveptr)) {
        GrblState state = grbl_state_from_string(report_token);
        if (state != GRBL_STATE_INVALID) {
            log_d("Grbl current state: %s", grbl_state_to_string(state));
            report.state = state;
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "WPos:")) {
            log_d("Work position: %s", report_token);
            report.w_pos = grbl_process_position(report_token);
            if (!(report.w_pos).is_set) {
                log_e("Could not parse work position");
            }
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "MPos:")) {
            log_d("Machine position: %s", report_token);
            report.m_pos = grbl_process_position(report_token);
            if (!report.m_pos.is_set) {
                log_e("Could not parse machine position");
            }
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "WCO:")) {
            log_d("Work Coordinate Offset: %s", report_token);
            report.wco = grbl_process_position(report_token);
            if (!report.wco.is_set) {
                log_e("Could not parse WCO");
            }
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "Bf:")) {
            log_d("Buffer state: %s", report_token);
            char *separator_ptr = strchr(report_token, ',');
            if (separator_ptr != NULL) {
                report.buffer_state.is_set = true;
                report.buffer_state.planned_buffer_available_blocks = atoi(report_token);
                report.buffer_state.rx_buffer_available_bytes = atoi(separator_ptr + 1);
            } else {
                log_e("Invalid Bf value: %s", report_token);
            }
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "Ln:")) {
            log_d("Line number: %s", report_token);
            report.line_number = atoi(report_token);
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "F:")) {
            log_d("F: %s", report_token);
            report.feed_state.is_set = true;
            report.feed_state.rate = atoi(report_token);
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "FS:")) {
            log_d("FS: %s", report_token);
            char *separator_ptr = strchr(report_token, ',');
            if (separator_ptr != NULL) {
                report.feed_state.is_set = true;
                report.feed_state.rate = atoi(report_token);
                report.feed_state.spindle_speed = atoi(separator_ptr + 1);
            } else {
                log_e("Invalid FS value: %s", report_token);
            }
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "Pn:")) {
            log_d("Triggered pins: %s", report_token);
            for (int pinIndex = 0; report_token[pinIndex] != '\0'; pinIndex++) {
                switch (report_token[pinIndex]) {
                case 'X':
                    report.active_pins += GRBL_PIN_FLAG_X;
                    break;
                case 'Y':
                    report.active_pins += GRBL_PIN_FLAG_Y;
                    break;
                case 'Z':
                    report.active_pins += GRBL_PIN_FLAG_Z;
                    break;
                case 'P':
                    report.active_pins += GRBL_PIN_FLAG_P;
                    break;
                case 'D':
                    report.active_pins += GRBL_PIN_FLAG_D;
                    break;
                case 'H':
                    report.active_pins += GRBL_PIN_FLAG_H;
                    break;
                case 'R':
                    report.active_pins += GRBL_PIN_FLAG_R;
                    break;
                case 'S':
                    report.active_pins += GRBL_PIN_FLAG_S;
                    break;
                default:
                    log_w("Unknown pin identifier %c", report_token[pinIndex]);
                }
            }
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "Ov:")) {
            log_d("Override values: %s", report_token);
            // TODO
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "A:")) {
            log_d("Accessory state: %s", report_token);
            // TODO
            continue;
        }

        log_d("Unsupported Grbl report token: %s", report_token);
    }

    grbl_update_last_report(&report);
}

static void grbl_process_feedback(const char *feedback_body) { log_d("Feedback body: %s", feedback_body); }

static void grbl_process_welcome(const char *welcome_line) {
    ui_overlay_add_flash_message(FLASH_LEVEL_INFO, welcome_line);
    log_d("Welcome message: %s", welcome_line);
    grbl_send_init_commands();
}

void grbl_process_line(char *line) {
    // Make sure we have at least one char
    if (line[0] == '\0') {
        log_d("Ignoring the current buffer since it was empty (probably caused by a CRLF being sent)");
        return;
    }

    log_d("Received Grbl data: %s", line);
    if (strcmp(line, "ok") == 0) {
        // ACK
        grbl_process_ack();
        return;
    }

    char *processed_line = line;

    if (grbl_remove_prefix_suffix(&processed_line, "error:")) {
        // Error
        grbl_process_error(processed_line);
        return;
    }

    if (grbl_remove_prefix_suffix(&processed_line, "ALARM:")) {
        // Error
        grbl_process_alarm(processed_line);
        return;
    }

    if (grbl_remove_prefix_suffix(&processed_line, "$")) {
        // Settings message
        grbl_process_settings(processed_line);
        return;
    }

    if (grbl_remove_prefix_suffix(&processed_line, "<", ">")) {
        // Status report
        grbl_process_report(processed_line);
        return;
    }

    if (grbl_remove_prefix_suffix(&processed_line, "[", "]")) {
        // Feedback message
        grbl_process_feedback(processed_line);
        return;
    }

    if (grbl_remove_prefix_suffix(&processed_line, "Grbl ")) {
        // Welcome message
        grbl_process_welcome(line);
        return;
    }

    log_d("Unsupported Grbl message: %s", line);
}
