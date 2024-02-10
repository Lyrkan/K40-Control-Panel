#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "tasks.h"

TaskHandle_t display_update_task_handle = (TaskHandle_t)NULL;
TaskHandle_t state_update_task_handle = (TaskHandle_t)NULL;
TaskHandle_t bed_update_task_handle = (TaskHandle_t)NULL;
TaskHandle_t grbl_rx_task_handle = (TaskHandle_t)NULL;
TaskHandle_t grbl_tx_task_handle = (TaskHandle_t)NULL;
TaskHandle_t settings_save_task_handle = (TaskHandle_t)NULL;
TaskHandle_t cpu_monitor_task_handle = (TaskHandle_t)NULL;
