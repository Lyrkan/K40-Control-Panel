#ifndef K40_ALERTS_H
#define K40_ALERTS_H

typedef enum {
    ALERT_TYPE_COOLING = 1 << 1,
    ALERT_TYPE_LIDS = 1 << 2,
    ALERT_TYPE_FLAME_SENSOR = 1 << 3,
    ALERT_TYPE_GRBL = 1 << 4,
} AlertType;

void alerts_toggle_alert(AlertType type, bool enable);
uint8_t alerts_get_current_alerts();

#endif
