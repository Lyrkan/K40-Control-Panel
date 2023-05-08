#ifndef API_H
#define API_H

#ifdef DEBUG
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
extern SemaphoreHandle_t webserver_screenshot_mutex;
#endif

void webserver_init();

#endif
