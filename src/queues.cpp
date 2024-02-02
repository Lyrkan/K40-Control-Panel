#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "Grbl/grbl_report.h"
#include "K40/bed.h"
#include "K40/relays.h"
#include "queues.h"

QueueHandle_t bed_current_status_queue = xQueueCreate(1, sizeof(BedStatus));
QueueHandle_t bed_command_queue = xQueueCreate(1, sizeof(BedCommand));
QueueHandle_t relays_command_queue = xQueueCreate(10, sizeof(RelaysCommand));
