#include <Arduino.h>
#include <sys/fcntl.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <ArduinoJson.h>

#include "esp_vfs_dev.h"
#include "driver/uart.h"

#include "Grbl/grbl_report.h"
#include "Grbl/grbl_serial.h"
#include "K40/alerts.h"
#include "K40/cooling.h"
#include "K40/flame_sensor.h"
#include "K40/lids.h"
#include "K40/relays.h"
#include "Headless/headless_parser.h"
#include "Headless/headless_serial.h"
#include "macros.h"
#include "mutex.h"
#include "queues.h"
#include "settings.h"
#include "tasks.h"

static SemaphoreHandle_t headless_serial_status_mutex = xSemaphoreCreateMutex();
static QueueHandle_t headless_tx_msg_queue = xQueueCreate(HEADLESS_TX_QUEUE_SIZE, sizeof(char *));

static void headless_rx_task(void *param) {
    char rx_line_buffer[HEADLESS_MAX_LINE_lENGTH + 1] = {0};
    int rx_line_buffer_idx = 0;
    bool rx_ignore_current_line = false;

    while (true) {
        int fd;
        if ((fd = open(HEADLESS_UART_FILE, O_RDONLY)) == -1) {
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
                                log_d(
                                    "Last instruction was ignored because its size exceeded HEADLESS_MAX_LINE_lENGTH");
                                rx_ignore_current_line = false;
                            } else {
                                headless_process_line(rx_line_buffer);
                            }

                            rx_line_buffer[rx_line_buffer_idx = 0] = '\0';
                            continue;
                        }

                        if (rx_ignore_current_line) {
                            continue;
                        }

                        if (rx_line_buffer_idx >= HEADLESS_MAX_LINE_lENGTH) {
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

static void headless_tx_task(void *param) {
    while (true) {
        int fd;
        if ((fd = open(HEADLESS_UART_FILE, O_WRONLY)) == -1) {
            log_e("Could not open headless UART in write-only mode, trying again in 2s (errno=%d)", errno);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        char *message;
        while (true) {
            if (xQueueReceive(headless_tx_msg_queue, &message, portMAX_DELAY) != pdTRUE) {
                log_e("Could not retrieve message from the headless TX queue");
                continue;
            }

            log_d("Sending message from headless TX queue: %s", message);
            if (write(fd, message, strnlen(message, HEADLESS_MAX_LINE_lENGTH)) == -1) {
                log_e("Could not write buffer to headless serial: %d", errno);
                free(message);
                break;
            }

            free(message);
        }

        close(fd);
    }
}

static void headless_status_update_task(void *param) {
    while (true) {
        StaticJsonDocument<512> payload;

        // Retrieve sensors data
        TAKE_MUTEX(cooling_current_status_mutex)
        payload["sensors"]["cooling"]["flow"]["in"] = cooling_values.input_flow;
        payload["sensors"]["cooling"]["flow"]["out"] = cooling_values.output_flow;
        payload["sensors"]["cooling"]["temp"]["in"] = cooling_values.input_temperature;
        payload["sensors"]["cooling"]["temp"]["out"] = cooling_values.output_temperature;
        RELEASE_MUTEX(cooling_current_status_mutex)

        TAKE_MUTEX(lids_current_status_mutex)
        payload["sensors"]["lids"]["front"] = lids_states.front_opened ? "opened" : "closed";
        payload["sensors"]["lids"]["back"] = lids_states.back_opened ? "opened" : "closed";
        RELEASE_MUTEX(lids_current_status_mutex)

        TAKE_MUTEX(flame_sensor_current_status_mutex)
        payload["sensors"]["flame_sensor"]["triggered"] = flame_sensor_triggered;
        RELEASE_MUTEX(flame_sensor_current_status_mutex)

        // Retrieve alerts
        uint8_t alerts_status = alerts_get_current_alerts();
        payload["alerts"]["cooling"] = (alerts_status & ALERT_TYPE_COOLING) != 0;
        payload["alerts"]["lids"] = (alerts_status & ALERT_TYPE_LIDS) != 0;
        payload["alerts"]["flame_sensor"] = (alerts_status & ALERT_TYPE_FLAME_SENSOR) != 0;

        // Retrieve relays state
        payload["relays"]["interlock"] = relays_is_active(RELAY_PIN_INTERLOCK);
        payload["relays"]["alarm"] = relays_is_active(RELAY_PIN_ALARM);
        payload["relays"]["lights"] = relays_is_active(RELAY_PIN_LIGHTS);
        payload["relays"]["beam_preview"] = relays_is_active(RELAY_PIN_BEAM_PREVIEW);

        // Retrieve UART status
        GrblSerialStatus serial_status = grbl_get_serial_status();
        payload["uart"] = serial_status;

        headless_send_message(HEADLESS_MESSAGE_TYPE_STATUS, payload);

        vTaskDelay(pdMS_TO_TICKS(HEADLESS_STATUS_UPDATE_INTERVAL_MS));
    }
}

void headless_serial_init() {
    // Set-up UART channel/pins
    const uart_config_t uart_config = {
        .baud_rate = HEADLESS_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_param_config(HEADLESS_UART_CHANNEL, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(HEADLESS_UART_CHANNEL, PIN_HEADLESS_TX, PIN_HEADLESS_RX, -1, -1));
    ESP_ERROR_CHECK(uart_driver_install(
        HEADLESS_UART_CHANNEL,
        HEADLESS_UART_RX_BUFFER_SIZE,
        HEADLESS_UART_TX_BUFFER_SIZE,
        0,
        NULL,
        0));

    // Set-up VFS to use UART driver
    esp_vfs_dev_uart_use_driver(HEADLESS_UART_CHANNEL);

    // Start TX/RX consumers
    xTaskCreatePinnedToCore(
        headless_rx_task,
        "headless_rx",
        TASK_HEADLESS_RX_STACK_SIZE,
        NULL,
        TASK_HEADLESS_RX_PRIORITY,
        &headless_rx_task_handle,
        TASK_HEADLESS_RX_CORE_ID);

    xTaskCreatePinnedToCore(
        headless_tx_task,
        "headless_tx",
        TASK_HEADLESS_TX_STACK_SIZE,
        NULL,
        TASK_HEADLESS_TX_PRIORITY,
        &headless_tx_task_handle,
        TASK_HEADLESS_TX_CORE_ID);

    xTaskCreatePinnedToCore(
        headless_status_update_task,
        "headless_status_update",
        TASK_HEADLESS_STATUS_UPDATE_STACK_SIZE,
        NULL,
        TASK_HEADLESS_STATUS_UPDATE_PRIORITY,
        &headless_status_update_task_handle,
        TASK_HEADLESS_STATUS_UPDATE_CORE_ID);
}

bool headless_send_message(HeadlessMessageType type, const JsonDocument &payload) {
    StaticJsonDocument<1024> doc;
    doc["t"] = type;
    doc["p"] = payload;

    char *message = (char *)malloc(sizeof(char) * (HEADLESS_MAX_LINE_lENGTH + 2));
    serializeJson(doc, message, HEADLESS_MAX_LINE_lENGTH);

    int message_length = strlen(message);
    message[message_length] = '\n';
    message[message_length + 1] = '\0';

    log_d("Scheduling headless message: %s", message);
    if (xQueueSendToBack(headless_tx_msg_queue, &message, 0) != pdTRUE) {
        log_w("Headless TX queue seems to be full, a message was dropped");
        return false;
    }

    return true;
}

bool headless_send_grbl_report() {
    StaticJsonDocument<1024> payload;

    TAKE_MUTEX(grbl_last_report_mutex)

    payload["state"] = grbl_last_report.state;
    payload["alarm"] = grbl_last_report.alarm;

    if (grbl_last_report.w_pos.is_set) {
        payload["w_pos"]["x"] = grbl_last_report.w_pos.x;
        payload["w_pos"]["y"] = grbl_last_report.w_pos.y;
        payload["w_pos"]["z"] = grbl_last_report.w_pos.z;
    } else {
        payload["w_pos"] = nullptr;
    }

    if (grbl_last_report.m_pos.is_set) {
        payload["m_pos"]["x"] = grbl_last_report.m_pos.x;
        payload["m_pos"]["y"] = grbl_last_report.m_pos.y;
        payload["m_pos"]["z"] = grbl_last_report.m_pos.z;
    } else {
        payload["m_pos"] = nullptr;
    }

    if (grbl_last_report.wco.is_set) {
        payload["wco"]["x"] = grbl_last_report.wco.x;
        payload["wco"]["y"] = grbl_last_report.wco.y;
        payload["wco"]["z"] = grbl_last_report.wco.z;
    } else {
        payload["wco"] = nullptr;
    }

    if (grbl_last_report.buffer_state.is_set) {
        payload["buffer"]["planned_buffer_available_blocks"] =
            grbl_last_report.buffer_state.planned_buffer_available_blocks;
        payload["buffer"]["rx_buffer_available_bytes"] = grbl_last_report.buffer_state.rx_buffer_available_bytes;
    } else {
        payload["buffer"] = nullptr;
    }

    if (grbl_last_report.feed_state.is_set) {
        payload["feed"]["rate"] = grbl_last_report.feed_state.rate;
        payload["feed"]["spindle_speed"] = grbl_last_report.feed_state.spindle_speed;
    } else {
        payload["feed"] = nullptr;
    }

    payload["line_number"] = grbl_last_report.line_number;

    int active_pins = grbl_last_report.active_pins;
    RELEASE_MUTEX(grbl_last_report_mutex)

    payload["active_pins"]["x"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_X) != 0;
    payload["active_pins"]["y"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_Y) != 0;
    payload["active_pins"]["z"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_Z) != 0;
    payload["active_pins"]["p"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_P) != 0;
    payload["active_pins"]["d"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_D) != 0;
    payload["active_pins"]["h"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_H) != 0;
    payload["active_pins"]["r"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_R) != 0;
    payload["active_pins"]["s"] = (grbl_last_report.active_pins & GRBL_PIN_FLAG_S) != 0;

    return headless_send_message(HEADLESS_MESSAGE_TYPE_GRBL_REPORT, payload);
}

bool headless_send_grbl_message(const char *message) {
    StaticJsonDocument<128> payload;
    payload["message"] = message;

    return headless_send_message(HEADLESS_MESSAGE_TYPE_GRBL_MESSAGE, payload);
}
