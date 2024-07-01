#ifndef __WIFI_CONFIG_H__
#define __WIFI_CONFIG_H__

#include <WiFi.h>

typedef enum {
    ACCESS_POINT,
    STATION,
}WIFI_MODE;

#define soft_ap_ssid  "ESP32-AP-Connect"
#define soft_ap_password  "12345678"

void WifiConnect();

#endif //__WIFI_CONFIG_H__