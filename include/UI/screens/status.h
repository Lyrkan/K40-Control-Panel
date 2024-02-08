#ifndef UI_SCREEN_STATUS_H
#define UI_SCREEN_STATUS_H

#include <lvgl.h>

#define STATUS_SYSTEM_UPDATE_INTERVAL 500

typedef enum {
    STATUS_UPDATE_PROBE_VOLTAGE = 1 << 0,
    STATUS_UPDATE_PROBE_COOLING = 1 << 1,
    STATUS_UPDATE_PROBE_LIDS = 1 << 2,
    STATUS_UPDATE_PROBE_FLAME_SENSOR = 1 << 3,
    STATUS_UPDATE_GRBL_REPORT = 1 << 4,
    STATUS_UPDATE_UART = 1 << 5,
} StatusUpdateType;

extern lv_obj_t *ui_status_screen;

void ui_status_init();
void ui_status_update(bool initialize = false);
void ui_status_notify_update(uint8_t update_types);

#endif
