#include <main.h>

WiFiServer server(80);

const char *soft_ap_ssid = "ESP32-AP-Connect";
const char *wifi_network_password = "12345678";
const char *wifi_network_ssid = "NDH";
const char *soft_ap_password = "12345678";
WiFiClient client;

void ConnectWifi() {
    uint8_t timeout = 100;
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(wifi_network_ssid);
    WiFi.begin(wifi_network_ssid, wifi_network_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        timeout--;
        if(timeout == 0) {
          Serial.println("WiFi connected timeout\n");
          Serial.println("Change to AP_Mode\n");
          goto AP_MODE;
        }
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    return;

AP_MODE:
    Serial.print("Setting AP (Access Point)…\n");
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    return;
}

void SPIFFS_Init() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File file = SPIFFS.open("/text.txt");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("File Content:");
  while(file.available()){
    Serial.println(file.read());
  }
  file.close();
}

void setup() {
  Serial.begin(115200);
  // ConnectWifi();
  // server.begin();

  SPIFFS_Init();
}

void loop() {
  client = server.available();
}