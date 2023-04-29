#ifndef K40_ALERTS_H
#define K40_ALERTS_H

enum AlertType {
    ALERT_TYPE_VOLTAGE = 0x01,
    ALERT_TYPE_COOLING = 0x02,
    ALERT_TYPE_LIDS = 0x04,
    ALERT_TYPE_FLAME_SENSOR = 0x08,
};

void alerts_toggle_alert(AlertType type, bool enable);
uint8_t alerts_get_current_alerts();

#endif
