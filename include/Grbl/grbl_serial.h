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
#define GRBL_ACK_DEFAULT_TIMEOUT_MS 5000
#define GRBL_ACK_HOMING_TIMEOUT_MS 30000
#define GRBL_TX_QUEUE_SIZE 10
#define GRBL_INIT_MESSAGES_DELAY_MS 5000
#define GRBL_MESSAGE_STATUS "?"
#define GRBL_MESSAGE_REPORT_FORMAT "$Report/Status=2"
#define GRBL_MESSAGE_REPORT_INTERVAL "$Report/Interval=200"

#define GRBL_TASK_NOTIFY_ACK_INDEX 0
#define GRBL_TASK_NOTIFY_ACK_SUCCESS 0
#define GRBL_TASK_NOTIFY_ACK_ERROR 1

typedef enum {
    PIN_GRBL_TX = 5,
    PIN_GRBL_RX = 34,
} GrblSerialPin;

typedef enum {
    GRBL_SERIAL_STATUS_CONNECTING,
    GRBL_SERIAL_STATUS_CONNECTED,
    GRBL_SERIAL_STATUS_DISCONNECTED,
} GrblSerialStatus;

typedef enum {
    GRBL_MOVE_MODE_UNDEFINED,
    GRBL_MOVE_MODE_RELATIVE,
    GRBL_MOVE_MODE_ABSOLUTE,
} GrblMoveMode;

typedef enum {
    GRBL_AXIS_X = 1 << 0,
    GRBL_AXIS_Y = 1 << 1,
    GRBL_AXIS_Z = 1 << 2,
} GrblAxis;

typedef struct {
    GrblMoveMode move_mode;
    float feed_rate;
    uint8_t axis_flags;
    float x;
    float y;
    float z;
} GrblMoveCommand;

typedef struct {
    void (*on_success)() = NULL;
    void (*on_failure)() = NULL;
    void (*on_finished)() = NULL;
} GrblCommandCallbacks;

typedef struct {
    char *buffer;
    uint32_t ack_timeout_ms;
    GrblCommandCallbacks callbacks;
} GrblCommand;

void grbl_serial_init();
GrblSerialStatus grbl_get_serial_status();
void grbl_set_serial_status(GrblSerialStatus serial_status);
bool grbl_send_message(
    const char *message,
    bool send_to_front = false,
    uint32_t ack_timeout = GRBL_ACK_DEFAULT_TIMEOUT_MS,
    GrblCommandCallbacks callbacks = GrblCommandCallbacks());
bool grbl_send_init_commands();
bool grbl_send_home_command(uint8_t axis_flags, GrblCommandCallbacks callbacks = GrblCommandCallbacks());
bool grbl_send_move_command(GrblMoveCommand command, GrblCommandCallbacks callbacks = GrblCommandCallbacks());

#endif
