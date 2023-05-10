#ifndef API_H
#define API_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t webserver_screenshot_mutex;

void webserver_init();

#endif
