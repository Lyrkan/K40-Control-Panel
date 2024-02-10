#ifndef TASKS_H
#define TASKS_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TASK_DISPLAY_UPDATE_STACK_SIZE 5120
#define TASK_STATE_UPDATE_STACK_SIZE 1500
#define TASK_BED_UPDATE_STACK_SIZE 2500
#define TASK_GRBL_RX_STACK_SIZE 2500
#define TASK_GRBL_TX_STACK_SIZE 3000
#define TASK_SETTINGS_SAVE_STACK_SIZE 1000
#define TASK_CPU_MONITOR_STACK_SIZE configMINIMAL_STACK_SIZE

#define TASK_DISPLAY_UPDATE_PRIORITY 1
#define TASK_STATE_UPDATE_PRIORITY 0
#define TASK_BED_UPDATE_PRIORITY 0
#define TASK_GRBL_RX_PRIORITY 1
#define TASK_GRBL_TX_PRIORITY 1
#define TASK_SETTINGS_PRIORITY 0
#define TASK_CPU_MONITOR_PRIORITY 0

#define TASK_DISPLAY_UPDATE_CORE_ID 1
#define TASK_STATE_UPDATE_CORE_ID 0
#define TASK_BED_UPDATE_CORE_ID 0
#define TASK_GRBL_RX_CORE_ID 0
#define TASK_GRBL_TX_CORE_ID 0
#define TASK_SETTINGS_CORE_ID 1
#define TASK_CPU_MONITOR_CORE_ID 1

extern TaskHandle_t display_update_task_handle;
extern TaskHandle_t state_update_task_handle;
extern TaskHandle_t bed_update_task_handle;
extern TaskHandle_t grbl_rx_task_handle;
extern TaskHandle_t grbl_tx_task_handle;
extern TaskHandle_t settings_save_task_handle;
extern TaskHandle_t cpu_monitor_task_handle;

#endif
