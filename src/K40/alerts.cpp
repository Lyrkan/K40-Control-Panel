#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "K40/alerts.h"
#include "queues.h"

static uint8_t alerts_status = 0;
static SemaphoreHandle_t alerts_status_mutex = xSemaphoreCreateMutex();

void alerts_toggle_alert(AlertType type, bool enable) {
    while (xSemaphoreTake(alerts_status_mutex, portMAX_DELAY) != pdTRUE)
        ;

    if (enable) {
        alerts_status |= type;
    } else {
        alerts_status &= ~type;
    }

    xSemaphoreGive(alerts_status_mutex);
}

uint8_t alerts_get_current_alerts() {
    while (xSemaphoreTake(alerts_status_mutex, portMAX_DELAY) != pdTRUE)
        ;

    uint8_t result = alerts_status;
    xSemaphoreGive(alerts_status_mutex);

    return result;
}
