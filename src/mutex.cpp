#include "mutex.h"

SemaphoreHandle_t cpu_monitor_stats_mutex = xSemaphoreCreateMutex();
SemaphoreHandle_t cooling_current_status_mutex = xSemaphoreCreateMutex();
SemaphoreHandle_t flame_sensor_current_status_mutex = xSemaphoreCreateMutex();
SemaphoreHandle_t lids_current_status_mutex = xSemaphoreCreateMutex();
SemaphoreHandle_t grbl_last_report_mutex = xSemaphoreCreateMutex();
