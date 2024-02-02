#ifndef K40_MUTEX_H
#define K40_MUTEX_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t cpu_monitor_stats_mutex;
extern SemaphoreHandle_t cooling_current_status_mutex;
extern SemaphoreHandle_t flame_sensor_current_status_mutex;
extern SemaphoreHandle_t lids_current_status_mutex;
extern SemaphoreHandle_t grbl_last_report_mutex;

#endif
