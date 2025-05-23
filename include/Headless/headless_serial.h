#ifndef HEADLESS_SERIAL_H
#define HEADLESS_SERIAL_H

#include <freertos/FreeRTOS.h>
#include <ArduinoJson.h>
#include "driver/uart.h"
#include "macros.h"

#define HEADLESS_UART_CHANNEL 2
#define HEADLESS_UART_FILE "/dev/uart/" STRINGIZE(HEADLESS_UART_CHANNEL)
#define HEADLESS_UART_BAUD_RATE 115200
#define HEADLESS_UART_RX_BUFFER_SIZE 2048
#define HEADLESS_UART_TX_BUFFER_SIZE 0
#define HEADLESS_MAX_LINE_lENGTH 1024
#define HEADLESS_TX_QUEUE_SIZE 10
#define HEADLESS_FORCED_UPDATE_INTERVAL_MS 500
#define HEADLESS_UPDATE_CHECK_INTERVAL_MS 50

typedef enum {
    PIN_HEADLESS_TX = 21,
    PIN_HEADLESS_RX = 18,
} HeadlessSerialPin;

typedef enum HeadlessMessageType {
    HEADLESS_MESSAGE_TYPE_STATUS = 0,
    HEADLESS_MESSAGE_TYPE_GRBL_REPORT = 1,
    HEADLESS_MESSAGE_TYPE_GRBL_MESSAGE = 2,
    HEADLESS_MESSAGE_TYPE_GRBL_ACK = 3,
    HEADLESS_MESSAGE_TYPE_SETTINGS = 4,
} HeadlessMessageType;

typedef enum {
    HEADLESS_UPDATE_STATUS = 1 << 0,
    HEADLESS_UPDATE_GRBL_REPORT = 1 << 1,
} HeadlessSerialUpdateType;

void headless_serial_init();
void headless_notify_update(uint8_t update_types);
bool headless_send_message(HeadlessMessageType type, const JsonDocument &payload);
bool headless_send_grbl_message(const char *message);

#endif
