#include <OnlineManage.h>
#include <HTML.h>
#include <File_System.h>


void OnlineManage::Init() {
  Serial.print("Connecting to ");
  Serial.println(online.wifi_setting.ssid);
  online.STA_Mode();
  if(CheckConnect()) {
    Serial.print("Connected!\n");
    online.Get_STA_IP();
  } else {
    Serial.println("Connection failed\n");
    Serial.println("Change to AP_Mode\n");
    online.AP_Mode();
    online.Get_AP_IP();
  }
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
bool OnlineManage::CheckConnect() {
  uint8_t timeout = 50;
  static long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (timeout == 0)
    {
      return false;
    }
    if (millis() - startTime >= 100) {
      startTime = millis();
      Serial.println(".");
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
  dataRead = savefile.readfile("/setting.json");
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
  savefile.writefile("/setting.json", setting, 0);
}

void OnlineManage::WebInit()
{
  online.server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", WebSocket_HTML); });

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    ESP.restart();
    request->send(200, "text/plain", "reset done"); });
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String Ssid;
    String Pass;
    if (request->hasParam(SSID_)) {
        Ssid = request->getParam(SSID_)->value();
    } else {
        Ssid = "khong co SSID";
    }
    wifisetting.ssid = Ssid;
    if (request->hasParam(PASS_)) {
        Pass = request->getParam(PASS_)->value();
    } else {
        Pass = "khong co SSID";
    }
    wifisetting.pass = Pass;
    
    request->send(200, "text/plain", "Hello, GET: " + Ssid + "|" + Pass);
    bool SetWifi = true;
    xQueueSend(qOnline, (void*) &SetWifi, 1/portTICK_PERIOD_MS); 
  });
}

void TaskOnlineManager(void *pvParameter)
{
  static bool SetWifi = false;
  qOnline = xQueueCreate(1, sizeof(bool));
  for (;;)
  {
    if (xQueueReceive(qOnline, (void *)&SetWifi, 1 / portTICK_PERIOD_MS) == pdTRUE)
    {
      WifiConnect();
      writeSetting();
      SetWifi = false;
    }
  }
}


void notifyClients() {
  ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      notifyClients();
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
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}


void OnlineManage::SocketSetup(); {
  initWebSocket();
}

void TaskSocketHandle(void *pvParameter) {
  for(;;) {
    ws.cleanupClients();
  }
}