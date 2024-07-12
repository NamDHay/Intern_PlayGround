#ifndef __ONLINE_MANAGE_H__
#define __ONLINE_MANAGE_H__

#include <WiFi.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

#include <HTML.h>
#include <File_System.h>
#include <IO_Config.h>

#define soft_ap_ssid "ESP32-AP-Connect"
#define soft_ap_password "12345678"

#define MAX_CLIENTS 4  // ESP32 supports up to 10 but I have not tested it yet
#define WIFI_CHANNEL 6 // 2.4ghz channel 6 https://en.wikipedia.org/wiki/List_of_WLAN_channels#2.4_GHz_(802.11b/g/n/ax)

#define UPDATE_INTERVAL_MS 60000
#define WIFI_STATUS_INTERVAL 2000
class OnlineManage
{
private:
public:
    bool isConnected = false;
    QueueHandle_t qWifiSetting;
    QueueHandle_t qPortalSetting;
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
    void PortalInit();

    void NTPInit();
    void printLocalTime(void);
};

extern OnlineManage online;

void TaskOnlineManager(void *pvParameter);

#endif //__ONLINE_MANAGE_H__