#include <Arduino.h>
#include <sys/fcntl.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "esp_vfs_dev.h"
#include "driver/uart.h"

#include "Grbl/grbl_parser.h"
#include "Grbl/grbl_serial.h"
#include "UI/overlay.h"
#include "UI/screens/status.h"
#include "macros.h"
#include "queues.h"
#include "tasks.h"

static GrblSerialStatus grbl_serial_status = GRBL_SERIAL_STATUS_DISCONNECTED;
static SemaphoreHandle_t grbl_serial_status_mutex = xSemaphoreCreateMutex();
static QueueHandle_t grbl_tx_msg_queue = xQueueCreate(GRBL_TX_QUEUE_SIZE, sizeof(char *));

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
        grbl_send_message(GRBL_MESSAGE_REPORT_INTERVAL, true);
        grbl_send_message(GRBL_MESSAGE_REPORT_FORMAT);
        vTaskDelay(pdMS_TO_TICKS(GRBL_INIT_MESSAGES_DELAY_MS));

        grbl_set_serial_status(GRBL_SERIAL_STATUS_CONNECTING);

        int fd;
        if ((fd = open(GRBL_UART_FILE, O_WRONLY)) == -1) {
            log_e("Could not open UART in write-only mode, trying again in 2s (errno=%d)", errno);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        char *msg_pointer = NULL;
        while (true) {
            if (xQueueReceive(grbl_tx_msg_queue, &msg_pointer, portMAX_DELAY) != pdTRUE) {
                log_e("Could not retrieve message from the TX queue");
                continue;
            }

            log_d("Sending message from TX queue: %s", msg_pointer);
            xTaskNotifyStateClearIndexed(NULL, GRBL_TASK_NOTIFY_ACK_INDEX);
            if (write(fd, msg_pointer, strnlen(msg_pointer, GRBL_MAX_LINE_lENGTH)) == -1) {
                log_e("Could not write buffer to serial: %d", errno);
                ui_overlay_add_flash_message(FLASH_LEVEL_DANGER, "An error happened when trying to send Grbl message");
            }

            if (xTaskNotifyWaitIndexed(
                    GRBL_TASK_NOTIFY_ACK_INDEX,
                    0x00,
                    ULONG_MAX,
                    NULL,
                    pdMS_TO_TICKS(GRBL_ACK_TIMEOUT_MS)) != pdTRUE) {
                if (!initialized) {
                    // Drop current message and the ones that were already in the queue
                    int dropped_messages = 1;
                    free(msg_pointer);
                    while (xQueueReceive(grbl_tx_msg_queue, &msg_pointer, 0) != pdFALSE) {
                        free(msg_pointer);
                        dropped_messages++;
                    }

                    log_d(
                        "Initial ack timed out, retrying in %d seconds (%d dropped messages)",
                        GRBL_INIT_MESSAGES_DELAY_MS,
                        dropped_messages);

                    goto reset;
                } else {
                    log_w("No ack received after %d milliseconds", GRBL_ACK_TIMEOUT_MS);

                    char flash_message[255];
                    snprintf(flash_message, ARRAY_SIZE(flash_message), "Grbl command timed-out: %s", msg_pointer);
                    flash_message[strnlen(flash_message, ARRAY_SIZE(flash_message)) - 1] = '\0'; // Remove line-ending
                    ui_overlay_add_flash_message(FLASH_LEVEL_WARNING, flash_message);
                }
            }

            free(msg_pointer);

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
    ui_status_notify_update(STATUS_UPDATE_UART);
}

bool grbl_send_message(const char *message, bool send_to_front) {
    size_t message_length = strlen(message);
    if (message_length > GRBL_MAX_LINE_lENGTH) {
        log_e("Message length exceeds GRBL_MAX_LINE_LENGTH: %s", message);
        ui_overlay_add_flash_message(
            FLASH_LEVEL_WARNING,
            "A Grbl message was ignored becaused it exceeded the max allowed length");
        return false;
    }

    char *message_copy = (char *)malloc(sizeof(char) * (message_length + 2));
    strlcpy(message_copy, message, message_length + 1);
    message_copy[message_length] = '\n';
    message_copy[message_length + 1] = '\0';

    if (send_to_front) {
        if (xQueueSendToFront(grbl_tx_msg_queue, &message_copy, 0) != pdTRUE) {
            log_w("TX queue seems to be full, a message was dropped");
            ui_overlay_add_flash_message(
                FLASH_LEVEL_WARNING,
                "A Grbl message was dropped because the TX queue seemed to be full");
            return false;
        }
    } else {
        if (xQueueSendToBack(grbl_tx_msg_queue, &message_copy, 0) != pdTRUE) {
            log_w("TX queue seems to be full, a message was dropped");
            ui_overlay_add_flash_message(
                FLASH_LEVEL_WARNING,
                "A Grbl message was dropped because the TX queue seemed to be full");
            return false;
        }
    }

    return true;
}

bool grbl_send_home_command(uint8_t axis_flags) {
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

    return grbl_send_message(buffer);
}

bool grbl_send_move_command(GrblMoveCoordinates target, GrblMoveMode mode) {
    log_i(
        "Sending a%s move command for axis %s%s%s with coordinates (%.2f, %.2f, %.2f)",
        mode == GRBL_MOVE_MODE_ABSOLUTE ? "n absolute"
                                        : (mode == GRBL_MOVE_MODE_RELATIVE ? " relative" : "n undefined"),
        (target.axis_flags & GRBL_AXIS_X) != 0 ? "X" : "",
        (target.axis_flags & GRBL_AXIS_Y) != 0 ? "Y" : "",
        (target.axis_flags & GRBL_AXIS_Z) != 0 ? "Z" : "",
        (target.axis_flags & GRBL_AXIS_X) != 0 ? target.x : 0,
        (target.axis_flags & GRBL_AXIS_Y) != 0 ? target.y : 0,
        (target.axis_flags & GRBL_AXIS_Z) != 0 ? target.z : 0);

    if (mode == GRBL_MOVE_MODE_ABSOLUTE || mode == GRBL_MOVE_MODE_RELATIVE) {
        bool switch_mode_scheduled = false;
        switch (mode) {
        case GRBL_MOVE_MODE_ABSOLUTE:
            if (!grbl_send_message("G90")) {
                return false;
            }
            break;
        case GRBL_MOVE_MODE_RELATIVE:
            if (!grbl_send_message("G91")) {
                return false;
            }
            break;
        }
    }

    char buffer[ARRAY_SIZE("G0 X000.00 Y000.00 Z000.00")] = "G0\0";
    if ((target.axis_flags & GRBL_AXIS_X) != 0) {
        snprintf(buffer, ARRAY_SIZE(buffer), "%s X%.2f", buffer, target.x);
    }
    if ((target.axis_flags & GRBL_AXIS_Y) != 0) {
        snprintf(buffer, ARRAY_SIZE(buffer), "%s Y%.2f", buffer, target.y);
    }
    if ((target.axis_flags & GRBL_AXIS_Z) != 0) {
        snprintf(buffer, ARRAY_SIZE(buffer), "%s Z%.2f", buffer, target.z);
    }

    return grbl_send_message(buffer);
}
