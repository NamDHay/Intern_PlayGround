#ifndef __MAIN_H__
#define __MAIN_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <WiFi.h>
// #include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

typedef enum {
    ACCESS_POINT,
    STATION,
}WIFI_MODE;

#endif