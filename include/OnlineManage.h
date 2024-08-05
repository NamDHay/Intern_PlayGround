#ifndef __ONLINE_MANAGE_H__
#define __ONLINE_MANAGE_H__

#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

#define soft_ap_ssid "ESP32-AP-Connect"
#define soft_ap_password "12345678"

#define UPDATE_INTERVAL_MS 500
#define WIFI_STATUS_INTERVAL 2000
class OnlineManage
{
private:
public:
    bool isConnected = false;
    QueueHandle_t qWifiSetting;
    String header;
    struct wifi_setting_t
    {
        String ssid;
        String pass;
        String waddress;
        String wgetway;
        String wsubnet;
        String staip;
        String wmode;
    } wifi_setting;

    void Init();
    void AP_Mode();
    void STA_Mode();
    void AP_STA_Mode();
    void Get_STA_IP();
    void Get_AP_IP();
    bool CheckConnect(uint32_t timeout);

    void loadSetting();
    void writeSetting();

    void WebHandle();
    void WebSocketInit();
    void notifyClients(const String &message);
};

extern OnlineManage online;

void TaskOnlineManager(void *pvParameter);

#endif //__ONLINE_MANAGE_H__