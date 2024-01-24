#include <Arduino.h>
#include <string.h>
#include <freertos/FreeRTOS.h>

#include "Grbl/grbl_parser.h"
#include "Grbl/grbl_serial.h"
#include "Grbl/grbl_state.h"
#include "UI/overlay.h"

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
        xTaskNotifyIndexed(grbl_tx_task_handle, GRBL_TASK_NOTIFY_ACK_INDEX, 0, eNoAction);
    }
}

static void grbl_process_error(const char *error_code) {
    int error_code_i = error_code[0] != '\0' ? atoi(error_code) : -1;
    const char *error_description = grbl_error_to_string((GrblError)error_code_i);

    log_e("Error message received (code: %d): %s", error_code_i, error_description);
    ui_overlay_add_flash_message(FLASH_LEVEL_DANGER, error_description);

    // Notify the TX task that its previous message has been acknowledged
    // TODO Differenciate acks from errors in the TX task
    if (grbl_tx_task_handle != NULL) {
        xTaskNotifyIndexed(grbl_tx_task_handle, GRBL_TASK_NOTIFY_ACK_INDEX, 0, eNoAction);
    }
}

static void grbl_process_alarm(const char *alarm_code) { log_e("Alarm triggered: %d", atoi(alarm_code)); }

static void grbl_process_settings(const char *settings_body) { log_d("Setting value: %s", settings_body); }

static void grbl_process_report(char *report_body) {
    log_d("Report: %s", report_body);

    char *report_token_saveptr;
    for (char *report_token = strtok_r(report_body, "|", &report_token_saveptr); report_token != NULL;
         report_token = strtok_r(NULL, "|", &report_token_saveptr)) {
        GrblState state = grbl_state_from_string(report_token);
        if (state != GRBL_STATE_INVALID) {
            log_i("Grbl current state: %s", grbl_state_to_string(state));
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "WPos:")) {
            log_i("Work position: %s", report_token);
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "MPos:")) {
            log_i("Machine position: %s", report_token);
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "Bf:")) {
            log_i("Buffer state: %s", report_token);
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "Ln:")) {
            log_i("Line number: %s", report_token);
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "F:")) {
            log_i("F: %s", report_token);
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "FS:")) {
            log_i("FS: %s", report_token);
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "Pn:")) {
            log_i("Triggered pins: %s", report_token);
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "Ov:")) {
            log_i("Override values: %s", report_token);
            continue;
        }

        if (grbl_remove_prefix_suffix(&report_token, "A:")) {
            log_i("Accessory state: %s", report_token);
            continue;
        }

        log_d("Unsupported Grbl report token: %s", report_token);
    }
}

static void grbl_process_feedback(const char *feedback_body) { log_d("Feedback body: %s", feedback_body); }

static void grbl_process_welcome(const char *welcome_line) {
    ui_overlay_add_flash_message(FLASH_LEVEL_INFO, welcome_line);
    log_d("Welcome message: %s", welcome_line);
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
