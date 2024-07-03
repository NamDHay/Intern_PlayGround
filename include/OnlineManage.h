#ifndef __ONLINE_MANAGE_H__
#define __ONLINE_MANAGE_H__

#include <WiFi.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

#define soft_ap_ssid  "ESP32-AP-Connect"
#define soft_ap_password  "12345678"

class OnlineManage {
private:
    QueueHandle_t qOnline;
    String header;
    const char *SSID_ = "ssid";
    const char *PASS_ = "pass";
    bool ledState = 0;
    
    void loadSetting();
    void writeSetting();
    void Get_STA_IP();
    void Get_AP_IP();
public:
    AsyncWebSocket ws("/ws");
    AsyncWebServer server(80);
    struct wifi_setting_t {
        String ssid;
        String pass;
    } wifi_setting;

    void Init();
    void AP_Mode();
    void STA_Mode();
    void AP_STA_Mode();
    bool CheckConnect();
    void Reconnect();

    void WebInit();
    void SocketSetup();
};

extern OnlineManage online;

void TaskOnlineManager(void *pvParameter);
void TaskSocketHandle(void *pvParameter);

#endif //__ONLINE_MANAGE_H__