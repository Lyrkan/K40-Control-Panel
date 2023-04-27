#ifndef K40_QUEUES_H
#define K40_QUEUES_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

extern QueueHandle_t bed_current_status_queue;
extern QueueHandle_t bed_command_queue;

extern QueueHandle_t cooling_status_update_queue;
extern QueueHandle_t flame_sensor_status_update_queue;
extern QueueHandle_t lids_status_update_queue;
extern QueueHandle_t voltage_probes_status_update_queue;

extern QueueHandle_t relays_current_status_queue;
extern QueueHandle_t relays_command_queue;

#endif
