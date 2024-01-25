#ifndef GRBL_SERIAL_H
#define GRBL_SERIAL_H

#include <freertos/FreeRTOS.h>

#include "driver/uart.h"
#include "macros.h"

#define GRBL_UART_CHANNEL 1
#define GRBL_UART_FILE "/dev/uart/" STRINGIZE(GRBL_UART_CHANNEL)
#define GRBL_UART_BAUD_RATE 115200
#define GRBL_UART_RX_BUFFER_SIZE 2048
#define GRBL_UART_TX_BUFFER_SIZE 0
#define GRBL_MAX_LINE_lENGTH 100
#define GRBL_ACK_TIMEOUT_MS 5000
#define GRBL_TX_QUEUE_SIZE 10
#define GRBL_INIT_MESSAGES_DELAY_MS 5000
#define GRBL_MESSAGE_REPORT_FORMAT "$Report/Status=2"
#define GRBL_MESSAGE_REPORT_INTERVAL "$Report/Interval=200"

#define GRBL_TASK_NOTIFY_ACK_INDEX 0

typedef enum {
    PIN_GRBL_TX = 5,
    PIN_GRBL_RX = 34,
} GrblSerialPin;

extern TaskHandle_t grbl_tx_task_handle;
extern TaskHandle_t grbl_rx_task_handle;

void grbl_serial_init();
bool grbl_send_message(const char *message, bool send_to_front = false);

#endif
