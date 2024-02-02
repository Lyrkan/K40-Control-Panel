#ifndef CPU_MONITOR_H
#define CPU_MONITOR_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <math.h>

extern float_t cpu_monitor_load_0;
extern float_t cpu_monitor_load_1;

void cpu_monitor_init();

#endif
