#ifndef UI_SCREEN_STATUS_H
#define UI_SCREEN_STATUS_H

#include <lvgl.h>

#define STATUS_SYSTEM_UPDATE_INTERVAL 500

typedef enum {
    STATUS_UPDATE_PROBE_VOLTAGE = 0x01,
    STATUS_UPDATE_PROBE_COOLING = 0x02,
    STATUS_UPDATE_PROBE_LIDS = 0x04,
    STATUS_UPDATE_PROBE_FLAME_SENSOR = 0x08,
} StatusUpdateType;

extern lv_obj_t *ui_status_screen;

void ui_status_init();
void ui_status_update(bool initialize = false);
void ui_status_notify_update(uint8_t update_types);

#endif
