#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "freertos/task.h"
#include "freertos/timers.h"
#include <math.h>

#include "cpu_monitor.h"

SemaphoreHandle_t cpu_monitor_stats_mutex = xSemaphoreCreateMutex();
float_t cpu_monitor_load_0 = 0;
float_t cpu_monitor_load_1 = 0;

static EventGroupHandle_t cpu_load_event_group = xEventGroupCreate();
;
static unsigned long idle_counter = 0;

static void cpu_monitor_idle_task(void *cpu_idle_bit) {
    unsigned long start;
    unsigned long end;

    while (true) {
        xEventGroupWaitBits(cpu_load_event_group, (int)cpu_idle_bit, false, false, portMAX_DELAY);
        start = millis();
        vTaskDelay(0 / portTICK_RATE_MS);
        end = millis();
        idle_counter += (end - start);
    }
}

static void cpu_monitor_task(void *param) {
    float cpu_percent;
    float adjust = 1.00;

    xEventGroupClearBits(cpu_load_event_group, 0x01);
    xEventGroupClearBits(cpu_load_event_group, 0x02);

    xTaskCreatePinnedToCore(
        &cpu_monitor_idle_task,
        "cpu_monitor_idle_task_cpu0",
        configMINIMAL_STACK_SIZE,
        (void *)0x01,
        0,
        NULL,
        0);

    xTaskCreatePinnedToCore(
        &cpu_monitor_idle_task,
        "cpu_monitor_idle_task_cpu1",
        configMINIMAL_STACK_SIZE,
        (void *)0x02,
        0,
        NULL,
        1);

    while (true) {
        // CPU 0
        idle_counter = 0;
        xEventGroupSetBits(cpu_load_event_group, 0x01);
        vTaskDelay(1000 / portTICK_RATE_MS);
        xEventGroupClearBits(cpu_load_event_group, 0x01);
        vTaskDelay(1 / portTICK_RATE_MS);

        xSemaphoreTake(cpu_monitor_stats_mutex, portMAX_DELAY);
        cpu_monitor_load_0 = max(0., 100 - idle_counter / 10.);
        xSemaphoreGive(cpu_monitor_stats_mutex);

        // CPU1
        idle_counter = 0;
        xEventGroupSetBits(cpu_load_event_group, 0x02);
        vTaskDelay(1000 / portTICK_RATE_MS);
        xEventGroupClearBits(cpu_load_event_group, 0x02);
        vTaskDelay(1 / portTICK_RATE_MS);

        xSemaphoreTake(cpu_monitor_stats_mutex, portMAX_DELAY);
        cpu_monitor_load_1 = max(0., 100 - idle_counter / 10.);
        xSemaphoreGive(cpu_monitor_stats_mutex);

        // Wait 10s for the next measurement
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void cpu_monitor_init() {
    xTaskCreatePinnedToCore(&cpu_monitor_task, "cpu_monitor_task", configMINIMAL_STACK_SIZE, NULL, 0, NULL, 0);
}
