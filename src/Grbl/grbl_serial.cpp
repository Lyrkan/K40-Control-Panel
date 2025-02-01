#include <Arduino.h>
#include <sys/fcntl.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "esp_vfs_dev.h"
#include "driver/uart.h"

#include "Grbl/grbl_parser.h"
#include "Grbl/grbl_serial.h"
#include "macros.h"
#include "queues.h"
#include "settings.h"
#include "tasks.h"

#if HAS_DISPLAY
#include "UI/overlay.h"
#include "UI/screens/status.h"
#else
#include "Headless/headless_serial.h"
#endif

static GrblSerialStatus grbl_serial_status = GRBL_SERIAL_STATUS_DISCONNECTED;
static SemaphoreHandle_t grbl_serial_status_mutex = xSemaphoreCreateMutex();
static QueueHandle_t grbl_tx_msg_queue = xQueueCreate(GRBL_TX_QUEUE_SIZE, sizeof(GrblCommand));

static void grbl_rx_task(void *param) {
    char rx_line_buffer[GRBL_MAX_LINE_lENGTH + 1] = {0};
    int rx_line_buffer_idx = 0;
    bool rx_ignore_current_line = false;

    while (true) {
        int fd;
        if ((fd = open(GRBL_UART_FILE, O_RDONLY)) == -1) {
            log_e("Could not open UART in read-only mode, trying again in 2s (errno=%d)", errno);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        while (true) {
            fd_set rfds;
            struct timeval tv = {
                .tv_sec = 5,
                .tv_usec = 0,
            };

            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            int s = select(fd + 1, &rfds, NULL, NULL, &tv);
            if (s < 0) {
                log_e("UART RX select failed: %d", errno);
                break;
            } else if (s == 0) {
                log_d("UART RX select timeout reached");
            } else {
                if (FD_ISSET(fd, &rfds)) {
                    char buf;
                    ssize_t read_bytes;
                    while ((read_bytes = read(fd, &buf, 1) > 0)) {
                        if (buf == '\n' || buf == '\r') {
                            if (rx_ignore_current_line) {
                                log_d("Last instruction was ignored because its size exceeded GRBL_MAX_LINE_lENGTH");
                                rx_ignore_current_line = false;
                            } else {
                                grbl_process_line(rx_line_buffer);
                            }

                            rx_line_buffer[rx_line_buffer_idx = 0] = '\0';
                            continue;
                        }

                        if (rx_ignore_current_line) {
                            continue;
                        }

                        if (rx_line_buffer_idx >= GRBL_MAX_LINE_lENGTH) {
                            log_e("Current buffer length exceeds GRBL_MAX_LINE_lENGTH, ignoring everything until the "
                                  "next line");
                            rx_ignore_current_line = true;
                            continue;
                        }

                        rx_line_buffer[rx_line_buffer_idx++] = buf;
                        rx_line_buffer[rx_line_buffer_idx] = '\0';
                    }

                    if (read_bytes < 0) {
                        log_e("UART RX read error: %d", errno);
                        break;
                    }
                } else {
                    log_e("UART RX select did not set file descriptor");
                    break;
                }
            }
        }

        close(fd);
    }
}

static void grbl_tx_task(void *param) {
    while (true) {
        grbl_set_serial_status(GRBL_SERIAL_STATUS_DISCONNECTED);

        // Schedule initial messages and then delay a bit
        // to make sure FluidNC had enough time to start
        bool initialized = false;
        grbl_send_init_commands();
        vTaskDelay(pdMS_TO_TICKS(GRBL_INIT_MESSAGES_DELAY_MS));

        grbl_set_serial_status(GRBL_SERIAL_STATUS_CONNECTING);

        int fd;
        if ((fd = open(GRBL_UART_FILE, O_WRONLY)) == -1) {
            log_e("Could not open UART in write-only mode, trying again in 2s (errno=%d)", errno);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        GrblCommand message;
        while (true) {
            if (xQueueReceive(grbl_tx_msg_queue, &message, portMAX_DELAY) != pdTRUE) {
                log_e("Could not retrieve message from the TX queue");
                continue;
            }

            log_d("Sending message from TX queue: %s", message.buffer);
            xTaskNotifyStateClearIndexed(NULL, GRBL_TASK_NOTIFY_ACK_INDEX);
            if (write(fd, message.buffer, strnlen(message.buffer, GRBL_MAX_LINE_lENGTH)) == -1) {
                log_e("Could not write buffer to serial: %d", errno);

#if HAS_DISPLAY
                ui_overlay_add_flash_message(FLASH_LEVEL_DANGER, "An error happened when trying to send Grbl message");
#endif
            }

            uint32_t ack_notification_value;
            if (xTaskNotifyWaitIndexed(
                    GRBL_TASK_NOTIFY_ACK_INDEX,
                    0x00,
                    ULONG_MAX,
                    &ack_notification_value,
                    pdMS_TO_TICKS(message.ack_timeout_ms)) != pdTRUE) {
                if (!initialized) {
                    // Drop current message
                    int dropped_messages = 1;
                    if (message.callbacks.on_failure != NULL) {
                        message.callbacks.on_failure();
                    }

                    if (message.callbacks.on_finished != NULL) {
                        message.callbacks.on_finished();
                    }

                    free(message.buffer);

                    // Drop remaining messages from the queue
                    while (xQueueReceive(grbl_tx_msg_queue, &message, 0) != pdFALSE) {
                        if (message.callbacks.on_failure != NULL) {
                            message.callbacks.on_failure();
                        }

                        if (message.callbacks.on_finished != NULL) {
                            message.callbacks.on_finished();
                        }

                        free(message.buffer);
                        dropped_messages++;
                    }

                    log_d(
                        "Initial ack timed out, retrying in %d seconds (%d dropped messages)",
                        GRBL_INIT_MESSAGES_DELAY_MS,
                        dropped_messages);

                    goto reset;
                } else {
                    log_w("No ack received after %d milliseconds", message.ack_timeout_ms);

                    char flash_message[255];
                    snprintf(flash_message, ARRAY_SIZE(flash_message), "Grbl command timed-out: %s", message.buffer);
                    flash_message[strnlen(flash_message, ARRAY_SIZE(flash_message)) - 1] = '\0'; // Remove line-ending

#if HAS_DISPLAY
                    ui_overlay_add_flash_message(FLASH_LEVEL_WARNING, flash_message);
#endif
                }

                // Notify sender that a timeout occurred
                if (message.callbacks.on_failure != NULL) {
                    message.callbacks.on_failure();
                }
            } else {
                // If we received a ack or an error for this message
                if (ack_notification_value == GRBL_TASK_NOTIFY_ACK_SUCCESS && message.callbacks.on_success != NULL) {
                    message.callbacks.on_success();
                } else if (
                    ack_notification_value == GRBL_TASK_NOTIFY_ACK_ERROR && message.callbacks.on_failure != NULL) {
                    message.callbacks.on_failure();
                }
            }

            // Notify sender that the command has been processed (doesn't matter if it succeeded or not)
            if (message.callbacks.on_finished != NULL) {
                message.callbacks.on_finished();
            }

            free(message.buffer);

            if (!initialized) {
                initialized = true;
                grbl_set_serial_status(GRBL_SERIAL_STATUS_CONNECTED);
            }
        }

    reset:
        close(fd);
    }
}
void grbl_serial_init() {
    // Set-up UART channel/pins
    const uart_config_t uart_config = {
        .baud_rate = GRBL_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_param_config(GRBL_UART_CHANNEL, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(GRBL_UART_CHANNEL, PIN_GRBL_TX, PIN_GRBL_RX, -1, -1));
    ESP_ERROR_CHECK(
        uart_driver_install(GRBL_UART_CHANNEL, GRBL_UART_RX_BUFFER_SIZE, GRBL_UART_TX_BUFFER_SIZE, 0, NULL, 0));

    // Set-up VFS to use UART driver
    esp_vfs_dev_uart_use_driver(GRBL_UART_CHANNEL);

    // Start TX/RX consumers
    xTaskCreatePinnedToCore(
        grbl_rx_task,
        "grbl_rx",
        TASK_GRBL_RX_STACK_SIZE,
        NULL,
        TASK_GRBL_RX_PRIORITY,
        &grbl_rx_task_handle,
        TASK_GRBL_RX_CORE_ID);

    xTaskCreatePinnedToCore(
        grbl_tx_task,
        "grbl_tx",
        TASK_GRBL_TX_STACK_SIZE,
        NULL,
        TASK_GRBL_TX_PRIORITY,
        &grbl_tx_task_handle,
        TASK_GRBL_TX_CORE_ID);
}

GrblSerialStatus grbl_get_serial_status() {
    GrblSerialStatus serial_status;
    TAKE_MUTEX(grbl_serial_status_mutex)
    serial_status = grbl_serial_status;
    RELEASE_MUTEX(grbl_serial_status_mutex)
    return serial_status;
}

void grbl_set_serial_status(GrblSerialStatus serial_status) {
    TAKE_MUTEX(grbl_serial_status_mutex)
    grbl_serial_status = serial_status;
    RELEASE_MUTEX(grbl_serial_status_mutex)

#if HAS_DISPLAY
    ui_status_notify_update(STATUS_UPDATE_UART);
#else
    headless_notify_update(HEADLESS_UPDATE_STATUS);
#endif
}

bool grbl_send_message(const char *message, GrblCommandCallbacks callbacks, bool send_to_front) {
    TAKE_MUTEX(grbl_settings_mutex)
    uint32_t timeout_ms = grbl_settings.default_timeout_ms;

    // Check if this is a homing command ($H...)
    if (strncmp(message, "$H", 2) == 0) {
        timeout_ms = grbl_settings.homing_timeout_ms;
    }

    RELEASE_MUTEX(grbl_settings_mutex);

    return grbl_send_message(message, timeout_ms, callbacks, send_to_front);
}

bool grbl_send_message(
    const char *message, uint32_t ack_timeout_ms, GrblCommandCallbacks callbacks, bool send_to_front) {
    size_t message_length = strlen(message);
    if (message_length > GRBL_MAX_LINE_lENGTH) {
        log_e("Message length exceeds GRBL_MAX_LINE_LENGTH: %s", message);

#if HAS_DISPLAY
        ui_overlay_add_flash_message(
            FLASH_LEVEL_WARNING,
            "A Grbl message was ignored becaused it exceeded the max allowed length");
#endif
        return false;
    }

    char *message_copy = (char *)malloc(sizeof(char) * (message_length + 2));
    strlcpy(message_copy, message, message_length + 1);
    message_copy[message_length] = '\n';
    message_copy[message_length + 1] = '\0';

    GrblCommand command = {
        .buffer = message_copy,
        .ack_timeout_ms = ack_timeout_ms,
        .callbacks = callbacks,
    };

    if (send_to_front) {
        log_d("Scheduling Grbl message with high priority (timeout: %dms): %s", command.ack_timeout_ms, command.buffer);
        if (xQueueSendToFront(grbl_tx_msg_queue, &command, 0) != pdTRUE) {
            log_w("TX queue seems to be full, a message was dropped");

#if HAS_DISPLAY
            ui_overlay_add_flash_message(
                FLASH_LEVEL_WARNING,
                "A Grbl message was dropped because the TX queue seemed to be full");
#endif

            return false;
        }
    } else {
        log_d("Scheduling Grbl message with low priority (timeout: %dms): %s", command.ack_timeout_ms, command.buffer);
        if (xQueueSendToBack(grbl_tx_msg_queue, &command, 0) != pdTRUE) {
            log_w("TX queue seems to be full, a message was dropped");

#if HAS_DISPLAY
            ui_overlay_add_flash_message(
                FLASH_LEVEL_WARNING,
                "A Grbl message was dropped because the TX queue seemed to be full");
#endif
            return false;
        }
    }

    return true;
}

bool grbl_send_init_commands() {
    log_d("Scheduling init commands (status / report interval / report format)");

    if (!grbl_send_message(GRBL_MESSAGE_STATUS, {}, true)) {
        log_e("Could not query current status");
        return false;
    }

    if (!grbl_send_message(GRBL_MESSAGE_REPORT_INTERVAL)) {
        log_e("Could not set automatic status report interval");
        return false;
    }

    if (!grbl_send_message(GRBL_MESSAGE_REPORT_FORMAT)) {
        log_e("Could not set status report format");
        return false;
    }

    return true;
}

bool grbl_send_home_command(uint8_t axis_flags, GrblCommandCallbacks callbacks) {
    log_d(
        "Sending a homing command for axis: %s%s%s",
        (axis_flags & GRBL_AXIS_X) != 0 ? "X" : "",
        (axis_flags & GRBL_AXIS_Y) != 0 ? "Y" : "",
        (axis_flags & GRBL_AXIS_Z) != 0 ? "Z" : "");

    char buffer[ARRAY_SIZE("$HXYZ")] = "$H\0";
    if ((axis_flags & GRBL_AXIS_X) != 0) {
        snprintf(buffer, ARRAY_SIZE(buffer), "%s%c", buffer, 'X');
    }
    if ((axis_flags & GRBL_AXIS_Y) != 0) {
        snprintf(buffer, ARRAY_SIZE(buffer), "%s%c", buffer, 'Y');
    }
    if ((axis_flags & GRBL_AXIS_Z) != 0) {
        snprintf(buffer, ARRAY_SIZE(buffer), "%s%c", buffer, 'Z');
    }

    return grbl_send_message(buffer, callbacks);
}

bool grbl_send_move_command(GrblMoveCommand command, GrblCommandCallbacks callbacks) {
    log_i(
        "Sending a%s move command for axis %s%s%s with coordinates (%.2f, %.2f, %.2f) at %.2fmm/s",
        command.move_mode == GRBL_MOVE_MODE_ABSOLUTE
            ? "n absolute"
            : (command.move_mode == GRBL_MOVE_MODE_RELATIVE ? " relative" : "n undefined"),
        (command.axis_flags & GRBL_AXIS_X) != 0 ? "X" : "",
        (command.axis_flags & GRBL_AXIS_Y) != 0 ? "Y" : "",
        (command.axis_flags & GRBL_AXIS_Z) != 0 ? "Z" : "",
        (command.axis_flags & GRBL_AXIS_X) != 0 ? command.x : 0,
        (command.axis_flags & GRBL_AXIS_Y) != 0 ? command.y : 0,
        (command.axis_flags & GRBL_AXIS_Z) != 0 ? command.z : 0,
        command.feed_rate);

    char buffer[ARRAY_SIZE("$J=G21 G90 F0000.0 X000.00 Y000.00 Z000.00")] = "$J=G21\0";

    // Set absolute/relative mode
    switch (command.move_mode) {
    case GRBL_MOVE_MODE_ABSOLUTE:
        snprintf(buffer, ARRAY_SIZE(buffer), "%s G90", buffer);
        break;
    case GRBL_MOVE_MODE_RELATIVE:
        snprintf(buffer, ARRAY_SIZE(buffer), "%s G91", buffer);
        break;
    }

    // Set feed rate
    snprintf(buffer, ARRAY_SIZE(buffer), "%s F%.1f", buffer, command.feed_rate * 60);

    // Set target
    if ((command.axis_flags & GRBL_AXIS_X) != 0) {
        snprintf(buffer, ARRAY_SIZE(buffer), "%s X%.2f", buffer, command.x);
    }
    if ((command.axis_flags & GRBL_AXIS_Y) != 0) {
        snprintf(buffer, ARRAY_SIZE(buffer), "%s Y%.2f", buffer, command.y);
    }
    if ((command.axis_flags & GRBL_AXIS_Z) != 0) {
        snprintf(buffer, ARRAY_SIZE(buffer), "%s Z%.2f", buffer, command.z);
    }

    return grbl_send_message(buffer, callbacks);
}

bool grbl_toogle_air_assist(bool enable, GrblCommandCallbacks callbacks) {
    return grbl_send_message(enable ? GRBL_MESSAGE_ENABLE_AIR_ASSIST : GRBL_MESSAGE_DISABLE_AIR_ASSIST, callbacks);
}
