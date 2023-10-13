#ifndef K40_FLAME_SENSOR_H
#define K40_FLAME_SENSOR_H

typedef enum {
    PIN_FLAME_SENSOR = 22
} FlameSensorPin;

void flame_sensor_update_status();

#endif
