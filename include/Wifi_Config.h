#ifndef __WIFI_CONFIG_H__
#define __WIFI_CONFIG_H__

#include <WiFi.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <BoardParameter.h>
typedef enum {
    ACCESS_POINT,
    STATION,
}WIFI_MODE;

#define soft_ap_ssid  "ESP32-AP-Connect"
#define soft_ap_password  "12345678"

void WifiConnect();
void AP_Mode();
void WebInit();
void loadSetting();
void writeSetting();
void TaskOnlineManager(void *pvParameter);
void TaskSocketHandle(void *pvParameter);
void SocketSetup();
#endif //__WIFI_CONFIG_H__