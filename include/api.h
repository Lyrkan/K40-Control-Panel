#ifndef API_H
#define API_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <ESPAsyncWebServer.h>

#ifdef DEBUG
extern SemaphoreHandle_t api_snapshot_mutex;
#endif

void api_init(AsyncWebServer *server);

#endif
