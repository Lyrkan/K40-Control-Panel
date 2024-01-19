#include <Arduino.h>
#include <string.h>
#include <freertos/FreeRTOS.h>

#include "Grbl/grbl_parser.h"
#include "Grbl/grbl_serial.h"

static bool grbl_check_type(char **input, const char *prefix, const char *suffix = NULL) {
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
    log_e("Error %d", atoi(error_code));

    // Notify the TX task that its previous message has been acknowledged
    // TODO Differenciate acks from errors in the TX task
    if (grbl_tx_task_handle != NULL) {
        xTaskNotifyIndexed(grbl_tx_task_handle, GRBL_TASK_NOTIFY_ACK_INDEX, 0, eNoAction);
    }
}

static void grbl_process_settings(const char *settings_body) { log_d("Setting value: %s", settings_body); }

static void grbl_process_report(const char *report_body) {
    log_d("Report: %s", report_body);
    grbl_send_message("foo");
}

static void grbl_process_feedback(const char *feedback_body) { log_d("Feedback body: %s", feedback_body); }

static void grbl_process_welcome(const char *welcome_line) { log_d("Welcome: %s", welcome_line); }

void grbl_process_line(char *line) {
    // Make sure we have at least one char
    if (line[0] == '\0') {
        log_d("Ignoring the current buffer since it was empty (probably caused by a CRLF being sent)");
        return;
    }

    log_i("Received Grbl data: %s", line);
    if (strncmp(line, "ok", 2) == 0) {
        // ACK
        grbl_process_ack();
        return;
    }

    char *processed_line = line;

    if (grbl_check_type(&processed_line, "error:")) {
        // Error
        grbl_process_error(processed_line);
        return;
    }

    if (grbl_check_type(&processed_line, "$")) {
        // Settings message
        grbl_process_settings(processed_line);
        return;
    }

    if (grbl_check_type(&processed_line, "<", ">")) {
        // Status report
        grbl_process_report(processed_line);
        return;
    }

    if (grbl_check_type(&processed_line, "[", "]")) {
        // Feedback message
        grbl_process_feedback(processed_line);
        return;
    }

    if (grbl_check_type(&processed_line, "Grbl ")) {
        // Welcome message
        grbl_process_welcome(line);
        return;
    }

    log_d("Unsupported Grbl message: %s", line);
}
