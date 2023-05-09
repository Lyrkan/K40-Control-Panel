#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "K40/alerts.h"
#include "macros.h"
#include "queues.h"

static uint8_t alerts_status = 0;
static SemaphoreHandle_t alerts_status_mutex = xSemaphoreCreateMutex();

void alerts_toggle_alert(AlertType type, bool enable) {
    TAKE_MUTEX(alerts_status_mutex)

    if (enable) {
        alerts_status |= type;
    } else {
        alerts_status &= ~type;
    }

    RELEASE_MUTEX(alerts_status_mutex)
}

uint8_t alerts_get_current_alerts() {
    TAKE_MUTEX(alerts_status_mutex)
    uint8_t result = alerts_status;
    RELEASE_MUTEX(alerts_status_mutex)

    return result;
}
