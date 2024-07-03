#ifndef __CAPTIVEPORTAL_H__
#define __CAPTIVEPORTAL_H__

#include <AsyncTCP.h>  //https://github.com/me-no-dev/AsyncTCP using the latest dev version from @me-no-dev
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>	//https://github.com/me-no-dev/ESPAsyncWebServer using the latest dev version from @me-no-dev
#include <esp_wifi.h>			//Used for mpdu_rx_disable android workaround

#define MAX_CLIENTS 4	// ESP32 supports up to 10 but I have not tested it yet
#define WIFI_CHANNEL 6	// 2.4ghz channel 6 https://en.wikipedia.org/wiki/List_of_WLAN_channels#2.4_GHz_(802.11b/g/n/ax)

const IPAddress localIP(192, 168, 4, 1);		   // the IP address the web server, Samsung requires the IP to be in public space
const IPAddress gatewayIP(192, 168, 4, 1);		   // IP address of the network should be the same as the local IP for captive portals
const IPAddress subnetMask(255, 255, 255, 0);  // no need to change: https://avinetworks.com/glossary/subnet-mask/

const String localIPURL = "http://192.168.4.1";	 // a string version of the local IP with http, used for redirecting clients to your webpage

class Portal {
private:
    
public:
    void Init();
};

extern Portal portal;

void PortalSetup(AsyncWebServer &server);
TaskHandle_t* GetPortalTaskHandle();
void TaskPortal(void *pvParameter);

#endif