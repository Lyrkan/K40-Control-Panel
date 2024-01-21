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

TaskHandle_t grbl_rx_task_handle = NULL;
TaskHandle_t grbl_tx_task_handle = NULL;

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
        int fd;

        if ((fd = open(GRBL_UART_FILE, O_WRONLY)) == -1) {
            log_e("Could not open UART in write-only mode, trying again in 2s (errno=%d)", errno);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        // Enable auto-report
        write(fd, GRBL_REPORT_INTERVAL "\n", ARRAY_SIZE(GRBL_REPORT_INTERVAL));

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
            }

            free(msg_pointer);

            if (xTaskNotifyWaitIndexed(
                    GRBL_TASK_NOTIFY_ACK_INDEX,
                    0x00,
                    ULONG_MAX,
                    NULL,
                    pdMS_TO_TICKS(GRBL_ACK_TIMEOUT_MS)) != pdTRUE) {
                log_w("No ack received after %d milliseconds", GRBL_ACK_TIMEOUT_MS);
            }
        }

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
        "grbl_rx_task",
        10000,
        NULL,
        0,
        &grbl_rx_task_handle,
        0 // Run on core #0, UI will be updated by loop() in core#1
    );

    xTaskCreatePinnedToCore(
        grbl_tx_task,
        "grbl_tx_task",
        10000,
        NULL,
        0,
        &grbl_tx_task_handle,
        0 // Run on core #0, UI will be updated by loop() in core#1
    );
}

bool grbl_send_message(const char *message) {
    size_t message_length = strlen(message);
    if (message_length > GRBL_MAX_LINE_lENGTH) {
        log_e("Message length exceeds GRBL_MAX_LINE_LENGTH");
        return false;
    }

    char *message_copy = (char *)malloc(sizeof(char) * (message_length + 2));
    strncpy(message_copy, message, message_length);
    message_copy[message_length] = '\n';
    message_copy[message_length + 1] = '\0';
    if (xQueueSendToBack(grbl_tx_msg_queue, &message_copy, 0) != pdTRUE) {
        log_w("TX queue seems to be full, a message was dropped");
        return false;
    }

    return true;
}
