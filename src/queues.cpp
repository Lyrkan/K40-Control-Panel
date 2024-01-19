#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "Grbl/grbl_serial.h"
#include "K40/bed.h"
#include "K40/cooling.h"
#include "K40/lids.h"
#include "K40/relays.h"
#include "queues.h"

QueueHandle_t bed_current_status_queue = xQueueCreate(1, sizeof(BedStatus));
QueueHandle_t bed_command_queue = xQueueCreate(1, sizeof(BedCommand));

QueueHandle_t cooling_current_status_queue = xQueueCreate(1, sizeof(CoolingValues));
QueueHandle_t flame_sensor_current_status_queue = xQueueCreate(1, sizeof(bool));
QueueHandle_t lids_current_status_queue = xQueueCreate(1, sizeof(LidsStates));

QueueHandle_t relays_command_queue = xQueueCreate(10, sizeof(RelaysCommand));
