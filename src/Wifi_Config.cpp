#include <Wifi_Config.h>

WiFiServer server(80);

struct wifi_setting_t {
  String ssid;
  String pass;
}wifisetting;

void WifiConnect() {
  uint8_t timeout = 100;
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(wifisetting.ssid);
  WiFi.begin(wifisetting.ssid, wifisetting.pass);
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      timeout--;
      if(timeout == 0) {
        Serial.println("WiFi connected timeout\n");
        Serial.println("Change to AP_Mode\n");
        goto AP_MODE;
      }
  }
  Serial.print("Connected!\n");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  return;
AP_MODE:
  WiFi.mode(WIFI_AP);
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
  return;
} //ConnectWifi