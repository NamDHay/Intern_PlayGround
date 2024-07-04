#include <OnlineManage.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

Website mywebsite;

const IPAddress local_IP(192, 168, 1, 128);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 0, 0);

const IPAddress localIP(192, 168, 4, 1);		   // the IP address the web server, Samsung requires the IP to be in public space
const IPAddress gatewayIP(192, 168, 4, 1);		   // IP address of the network should be the same as the local IP for captive portals
const IPAddress subnetMask(255, 255, 255, 0);  // no need to change: https://avinetworks.com/glossary/subnet-mask/

const String localIPURL = "http://192.168.4.1/setting";	 // a string version of the local IP with http, used for redirecting clients to your webpage

const char* ntpServer = "vn.pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void OnlineManage::Init() {
  online.loadSetting();
  Serial.print("Connecting to ");
  Serial.println(online.wifi_setting.ssid);
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  online.AP_STA_Mode();
  if(online.CheckConnect(500)) {
    Serial.print("Connected!\n");
    online.Get_STA_IP();
  } else {
    Serial.println("Connection failed\n");
    Serial.println("Change to AP_Mode\n");
    online.AP_Mode();
    online.Get_AP_IP();
  }
  online.NTPInit();
  online.WebSocketInit();
  online.PortalInit();
  online.WebHandle();
  server.begin();
}
void OnlineManage::AP_Mode()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
}
void OnlineManage::STA_Mode() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(online.wifi_setting.ssid, online.wifi_setting.pass);
}
void OnlineManage::Get_STA_IP() {
  Serial.print("STA -> IP address: ");
  Serial.println(WiFi.localIP());
}
void OnlineManage::Get_AP_IP() {
  Serial.print("AP -> IP address: ");
  Serial.println(WiFi.softAPIP());
}
void OnlineManage::AP_STA_Mode() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  WiFi.begin(online.wifi_setting.ssid, online.wifi_setting.pass);
}
bool OnlineManage::CheckConnect(uint32_t timeout) {
  static long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (timeout == 0)
    {
      return false;
    }
    if (millis() - startTime >= 10) {
      startTime = millis();
      timeout--;
    }
  }
  return true;
}
void OnlineManage::Reconnect()
{

}

void OnlineManage::loadSetting()
{
  String dataRead = "";
  JsonDocument doc;
  dataRead = filesystem.readfile("/setting.json");
  deserializeJson(doc, dataRead);
  String SSID = doc["SSID"].as<String>();
  String PASS = doc["PASS"].as<String>();
  Serial.println("SSID: " + SSID);
  Serial.println("PASS: " + PASS);
  online.wifi_setting.ssid = SSID;
  online.wifi_setting.pass = PASS;
}
void OnlineManage::writeSetting()
{
  String setting = "";
  JsonDocument writeDoc;
  writeDoc["SSID"] = online.wifi_setting.ssid;
  writeDoc["PASS"] = online.wifi_setting.pass;
  serializeJson(writeDoc, setting);
  Serial.println("JSON: " + setting);
  filesystem.writefile("/setting.json", setting, 0);
}

#define DNS_INTERVAL 30
DNSServer dnsServer;
void OnlineManage::PortalInit() {
	dnsServer.setTTL(3600);
	dnsServer.start(53, "*", WiFi.softAPIP());

	Serial.print("\n");
	Serial.print("Startup Time:");	// should be somewhere between 270-350 for Generic ESP32 (D0WDQ6 chip, can have a higher startup time on first boot)
	Serial.println(millis());
	Serial.print("\n");
}

void notifyClients(const String &message) {
  ws.textAll(message);
}
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      digitalWrite(LED, !digitalRead(LED));
      notifyClients(String(digitalRead(LED)));
    }
    else {
      String DataStr = "";
      for(int i = 0 ; i < len ; i++){DataStr += (char)data[i];}
      JsonDocument doc;
      deserializeJson(doc, DataStr);
      String SSID = doc["SSID"].as<String>();
      String PASS = doc["PASS"].as<String>();
      Serial.println("SSID: " + SSID);
      Serial.println("PASS: " + PASS);
      online.wifi_setting.ssid = SSID;
      online.wifi_setting.pass = PASS;
      online.writeSetting();
      bool IsMessage = true;
      xQueueSend(online.qWebSocket, (void*) &IsMessage, 1/portTICK_PERIOD_MS); 
    }
  }
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
  }
}
void OnlineManage::WebSocketInit() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

const char *SSID_ = "ssid";
const char *PASS_ = "pass";
void OnlineManage::WebHandle() {
	server.on("/connecttest.txt", [](AsyncWebServerRequest *request) { request->redirect("http://logout.net"); });	// windows 11 captive portal workaround
	server.on("/wpad.dat", [](AsyncWebServerRequest *request) { request->send(404); });								// Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

	server.on("/generate_204", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });		   // android captive portal redirect
	server.on("/redirect", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // microsoft redirect
	server.on("/hotspot-detect.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });  // apple call home
	// server.on("/canonical.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });	   // firefox captive portal call home
	// server.on("/success.txt", [](AsyncWebServerRequest *request) { request->send(200); });					   // firefox captive portal call home
	server.on("/chrome-variations/seed",[](AsyncWebServerRequest *request){request->send(200);}); //chrome captive portal call home
	// server.on("/ncsi.txt", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // windows call home

	server.on("/setting", HTTP_ANY, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse(200, "text/html", mywebsite.Wifi_Config_HTML);
		request->send(response);
		Serial.println("Served Wifi Config Page");
	});
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String Ssid;
    String Pass;
    if (request->hasParam(SSID_)) {
        Ssid = request->getParam(SSID_)->value();
    } else {
        Ssid = "khong co SSID";
    }
    online.wifi_setting.ssid = Ssid;
    if (request->hasParam(PASS_)) {
        Pass = request->getParam(PASS_)->value();
    } else {
        Pass = "khong co SSID";
    }
    online.wifi_setting.pass = Pass;
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", mywebsite.Wifi_Config_HTML);
    request->send(response);
    bool SetWifi = true;
    xQueueSend(online.qOnline, (void*) &SetWifi, 1/portTICK_PERIOD_MS); 
  });
}

void OnlineManage::NTPInit() {
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "asia.pool.ntp.org");
  return;
}
void OnlineManage::printLocalTime() {
  time_t now;
  struct tm timeinfo;
  time(&now);
  gmtime_r(&now, &timeinfo);
  timeinfo.tm_hour += 7;
  timeinfo.tm_hour %= 24;
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void TaskOnlineManager(void *pvParameter)
{
  static long startNPTTime = millis();
  static long startPortalTime = millis();
  static long startCheckWifiTime = millis();
  static bool SetWifi = false;
  static bool IsMessageReceived = false;
  online.qOnline = xQueueCreate(1, sizeof(bool));
  online.qWebSocket = xQueueCreate(1, sizeof(bool));
  for (;;)
  {
    if (xQueueReceive(online.qOnline, (void *)&SetWifi, 1 / portTICK_PERIOD_MS) == pdTRUE)
    {
      WiFi.begin(online.wifi_setting.ssid, online.wifi_setting.pass);
      online.writeSetting();
      SetWifi = false;
      Serial.print("Im in queue Portal Send seting\n");
    }
    if (xQueueReceive(online.qWebSocket, (void *)&IsMessageReceived, 1 / portTICK_PERIOD_MS) == pdTRUE) {
      WiFi.begin(online.wifi_setting.ssid, online.wifi_setting.pass);
      IsMessageReceived = false;
      Serial.print("Im in queue Socket\n");
    }
    // if (millis() - startNPTTime >= 2000) {
    //   startNPTTime = millis();        
    //   online.printLocalTime();
    // }
    if(millis() - startCheckWifiTime >= 10){
      startCheckWifiTime = millis();
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wifi Disconnected");
        dnsServer.processNextRequest();	
      }
    }
    ws.cleanupClients();
  }
}