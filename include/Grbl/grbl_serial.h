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
#define GRBL_TX_QUEUE_SIZE 10
#define GRBL_INIT_MESSAGES_DELAY_MS 5000
#define GRBL_MESSAGE_STATUS "?"
#define GRBL_MESSAGE_REPORT_FORMAT "$Report/Status=2"
#define GRBL_MESSAGE_REPORT_INTERVAL "$Report/Interval=200"

// TODO Allow to configure which one of M7 or M8 should be used
#define GRBL_MESSAGE_ENABLE_AIR_ASSIST "M7"
#define GRBL_MESSAGE_DISABLE_AIR_ASSIST "M9"

#define GRBL_TASK_NOTIFY_ACK_INDEX 0
#define GRBL_TASK_NOTIFY_ACK_SUCCESS 0
#define GRBL_TASK_NOTIFY_ACK_ERROR 1

typedef enum {
    PIN_GRBL_TX = 5,
    PIN_GRBL_RX = 34,
} GrblSerialPin;

typedef enum {
    GRBL_SERIAL_STATUS_CONNECTING = 0,
    GRBL_SERIAL_STATUS_CONNECTED = 1,
    GRBL_SERIAL_STATUS_DISCONNECTED = 2,
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
    const char *message, GrblCommandCallbacks callbacks = GrblCommandCallbacks(), bool send_to_front = false);
bool grbl_send_message(
    const char *message,
    uint32_t ack_timeout,
    GrblCommandCallbacks callbacks = GrblCommandCallbacks(),
    bool send_to_front = false);

bool grbl_send_init_commands();
bool grbl_send_home_command(uint8_t axis_flags, GrblCommandCallbacks callbacks = GrblCommandCallbacks());
bool grbl_send_move_command(GrblMoveCommand command, GrblCommandCallbacks callbacks = GrblCommandCallbacks());
bool grbl_toogle_air_assist(bool enable, GrblCommandCallbacks callbacks = GrblCommandCallbacks());

#endif
