#include <freertos/FreeRTOS.h>
#include "esp.h"

const char *esp_reset_reason_as_text() {
    switch (esp_reset_reason()) {
    case ESP_RST_POWERON:
        return "Power-on event";
    case ESP_RST_EXT:
        return "Reset by external pin";
    case ESP_RST_SW:
        return "Software reset via esp_restart";
    case ESP_RST_PANIC:
        return "Software reset due to exception/panic";
    case ESP_RST_INT_WDT:
        return "Interrupt watchdog";
    case ESP_RST_TASK_WDT:
        return "Task watchdog";
    case ESP_RST_WDT:
        return "Watchdog other than interrupt/task";
    case ESP_RST_DEEPSLEEP:
        return "Deep sleep exit";
    case ESP_RST_BROWNOUT:
        return "Brownout reset";
    case ESP_RST_SDIO:
        return "Reset over SDIO";
    }

    return "Unknown";
}
