// #include <File_System.h>
#include <Wifi_Config.h>
// #include <CaptivePortal.h>


struct wifi_setting_t
{
  String ssid;
  String pass;
} wifisetting;
QueueHandle_t qOnline;
String header;
const char *SSID_ = "ssid";
const char *PASS_ = "pass";
AsyncWebSocket ws("/ws");
bool ledState = 0;

void WifiConnect()
{
  uint8_t timeout = 50;
  static long startTime = millis();
  WiFi.mode(WIFI_AP_STA);
  Serial.print("Connecting to ");
  Serial.println(wifisetting.ssid);
  WiFi.begin(wifisetting.ssid, wifisetting.pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    if (timeout == 0)
    {
      Serial.println("\nWiFi connected timeout\n");
      Serial.println("Change to AP_Mode\n");
      AP_Mode();
      return;
    }
    if (millis() - startTime >= 100) {
      startTime = millis();
      Serial.println(".");
      timeout--;
    }
  }
  Serial.print("Connected!\n");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  SocketSetup();
  // server.begin();
  PortalSetup(server);
  return;
}
void AP_Mode()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  PortalSetup(server);
  return;
}

void loadSetting()
{
  String dataRead = "";
  JsonDocument doc;
  dataRead = readfile("/setting.json");
  deserializeJson(doc, dataRead);
  String SSID = doc["SSID"].as<String>();
  String PASS = doc["PASS"].as<String>();
  Serial.println("SSID: " + SSID);
  Serial.println("PASS: " + PASS);
  wifisetting.ssid = SSID;
  wifisetting.pass = PASS;
}
void writeSetting()
{
  String setting = "";
  JsonDocument writeDoc;
  writeDoc["SSID"] = wifisetting.ssid;
  writeDoc["PASS"] = wifisetting.pass;
  serializeJson(writeDoc, setting);
  Serial.println("JSON: " + setting);
  writefile("/setting.json", setting, 0);
}

void WebInit()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
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

void SocketSetup() {
  initWebSocket();
}

void TaskSocketHandle(void *pvParameter) {
  for(;;) {
    ws.cleanupClients();
    digitalWrite(ledPin, ledState);
  }
}