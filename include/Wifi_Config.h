#ifndef __WIFI_CONFIG_H__
#define __WIFI_CONFIG_H__

#include <WiFi.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

typedef enum {
    ACCESS_POINT,
    STATION,
}WIFI_MODE;

#define soft_ap_ssid  "ESP32-AP-Connect"
#define soft_ap_password  "12345678"


void WifiConnect();
void AP_Mode();
void WebInit();
void TaskOnlineManager(void *pvParameter);
void loadSetting();
void writeSetting();

#endif //__WIFI_CONFIG_H__