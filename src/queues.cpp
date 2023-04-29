#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "K40/bed.h"
#include "K40/cooling.h"
#include "K40/lids.h"
#include "K40/voltage_probes.h"
#include "K40/relays.h"
#include "queues.h"

QueueHandle_t bed_current_status_queue = xQueueCreate(1, sizeof(BedStatus));
QueueHandle_t bed_command_queue = xQueueCreate(1, sizeof(BedCommand));

QueueHandle_t cooling_current_status_queue = xQueueCreate(1, sizeof(CoolingValues));
QueueHandle_t flame_sensor_current_status_queue = xQueueCreate(1, sizeof(bool));
QueueHandle_t lids_current_status_queue = xQueueCreate(1, sizeof(LidsStates));
QueueHandle_t voltage_current_status_queue = xQueueCreate(1, sizeof(VoltageProbesValues));

QueueHandle_t relays_current_status_queue = xQueueCreate(1, sizeof(RelaysStatus));
QueueHandle_t relays_command_queue = xQueueCreate(10, sizeof(RelaysCommand));
