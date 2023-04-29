#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "K40/alerts.h"
#include "queues.h"

static uint8_t alerts_statuses = 0;
static SemaphoreHandle_t alerts_statuses_mutex = xSemaphoreCreateMutex();

void alerts_toggle_alert(AlertType type, bool enable) {
    while (xSemaphoreTake(alerts_statuses_mutex, portMAX_DELAY) != pdPASS)
        ;

    if (enable) {
        alerts_statuses |= type;
    } else {
        alerts_statuses &= ~type;
    }

    xSemaphoreGive(alerts_statuses_mutex);
}

uint8_t alerts_get_current_alerts() {
    while (xSemaphoreTake(alerts_statuses_mutex, portMAX_DELAY) != pdPASS)
        ;

    uint8_t result = alerts_statuses;
    xSemaphoreGive(alerts_statuses_mutex);

    return result;
}
