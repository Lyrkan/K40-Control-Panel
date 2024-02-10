#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_freertos_hooks.h"
#include <math.h>

#include "cpu_monitor.h"
#include "macros.h"
#include "mutex.h"
#include "tasks.h"

#define CPU_MONITOR_REFRESH_INTERVAL_MS 1000
#define CPU_MONITOR_EXPECTED_IDLE_TICKS_MULTIPLIER 4.5 // TODO Adjust this to get more accurate values
#define CPU_MONITOR_EXPECTED_IDLE_TICKS_PER_MS                                                                         \
    (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * CPU_MONITOR_EXPECTED_IDLE_TICKS_MULTIPLIER)

float_t cpu_monitor_load_0 = 0;
float_t cpu_monitor_load_1 = 0;

static unsigned long idle_counter_0 = 0;
static unsigned long idle_counter_1 = 0;

static bool cpu_monitor_idle_task_0() {
    idle_counter_0++;
    return false;
}
static bool cpu_monitor_idle_task_1() {
    idle_counter_1++;
    return false;
}

static void cpu_monitor_task(void *param) {
    esp_register_freertos_idle_hook_for_cpu(cpu_monitor_idle_task_0, 0);
    esp_register_freertos_idle_hook_for_cpu(cpu_monitor_idle_task_1, 1);

    unsigned long start_time = millis();
    unsigned long idle_result_0;
    unsigned long idle_result_1;
    unsigned long expected_ticks;

    while (true) {
        idle_result_0 = idle_counter_0;
        idle_result_1 = idle_counter_1;
        expected_ticks = CPU_MONITOR_EXPECTED_IDLE_TICKS_PER_MS * (millis() - start_time);

        if (expected_ticks == 0) {
            continue;
        }

        // CPU 0
        TAKE_MUTEX(cpu_monitor_stats_mutex)
        cpu_monitor_load_0 = 100.f - (100.f * (min(expected_ticks, idle_result_0) / (float)expected_ticks));
        RELEASE_MUTEX(cpu_monitor_stats_mutex)

        // CPU1
        TAKE_MUTEX(cpu_monitor_stats_mutex)
        cpu_monitor_load_1 = 100.f - (100.f * (min(expected_ticks, idle_result_1) / (float)expected_ticks));
        RELEASE_MUTEX(cpu_monitor_stats_mutex)

        // Wait 5s for the next measurement
        start_time = millis();
        idle_counter_0 = 0;
        idle_counter_1 = 0;
        vTaskDelay(pdMS_TO_TICKS(CPU_MONITOR_REFRESH_INTERVAL_MS));
    }
}

void cpu_monitor_init() {
    xTaskCreatePinnedToCore(
        &cpu_monitor_task,
        "cpu_monitor",
        TASK_CPU_MONITOR_STACK_SIZE,
        NULL,
        TASK_CPU_MONITOR_PRIORITY,
        &cpu_monitor_task_handle,
        TASK_CPU_MONITOR_CORE_ID);
}
