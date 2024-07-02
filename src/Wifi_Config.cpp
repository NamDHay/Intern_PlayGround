#include <File_System.h>
#include <Wifi_Config.h>
#include <CaptivePortal.h>

AsyncWebServer server(80);
QueueHandle_t qOnline;

struct wifi_setting_t
{
  String ssid;
  String pass;
} wifisetting;

String header;
String HTML = "\
<!DOCTYPE HTML><html>\
<head>\
  <title>ESP Web Server</title>\
  <meta name='viewport' content='width=device-width, initial-scale=1'>\
  <link rel='icon' href='data:,'>\
  <style>\
  html {\
    font-family: Arial, Helvetica, sans-serif;\
    text-align: center;\
  }\
  h1 {\
    font-size: 1.8rem;\
    color: white;\
  }\
  h2{\
    font-size: 1.5rem;\
    font-weight: bold;\
    color: #143642;\
  }\
  .topnav {\
    overflow: hidden;\
    background-color: #143642;\
  }\
  body {\
    margin: 0;\
  }\
  .content {\
    padding: 30px;\
    max-width: 600px;\
    margin: 0 auto;\
  }\
  .card {\
    background-color: #F8F7F9;;\
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);\
    padding-top:10px;\
    padding-bottom:20px;\
  }\
  .button {\
    padding: 15px 50px;\
    font-size: 24px;\
    text-align: center;\
    outline: none;\
    color: #fff;\
    background-color: #0f8b8d;\
    border: none;\
    border-radius: 5px;\
    -webkit-touch-callout: none;\
    -webkit-user-select: none;\
    -khtml-user-select: none;\
    -moz-user-select: none;\
    -ms-user-select: none;\
    user-select: none;\
    -webkit-tap-highlight-color: rgba(0,0,0,0);\
   }\
   /*.button:hover {background-color: #0f8b8d}*/\
   .button:active {\
     background-color: #0f8b8d;\
     box-shadow: 2 2px #CDCDCD;\
     transform: translateY(2px);\
   }\
   .state {\
     font-size: 1.5rem;\
     color:#8c8c8c;\
     font-weight: bold;\
   }\
  </style>\
<title>ESP Web Server</title>\
<meta name='viewport' content='width=device-width, initial-scale=1'>\
<link rel='icon' href='data:,'>\
</head>\
<body>\
  <div class='topnav'>\
    <h1>ESP WebSocket Server</h1>\
  </div>\
  <div class='content'>\
    <div class='card'>\
      <h2>Output - GPIO 2</h2>\
      <p class='state'>state: <span id='state'>%STATE%</span></p>\
      <p><button id='button' class='button'>Toggle</button></p>\
    </div>\
  </div>\
<script>\
  var gateway = `ws://${window.location.hostname}/ws`;\
  var websocket;\
  window.addEventListener('load', onLoad);\
  function initWebSocket() {\
    console.log('Trying to open a WebSocket connection...');\
    websocket = new WebSocket(gateway);\
    websocket.onopen    = onOpen;\
    websocket.onclose   = onClose;\
    websocket.onmessage = onMessage;\
  }\
  function onOpen(event) {\
    console.log('Connection opened');\
  }\
  function onClose(event) {\
    console.log('Connection closed');\
    setTimeout(initWebSocket, 2000);\
  }\
  function onMessage(event) {\
    var state;\
    if (event.data == '1'){\
      state = 'ON';\
    }\
    else{\
      state = 'OFF';\
    }\
    document.getElementById('state').innerHTML = state;\
  }\
  function onLoad(event) {\
    initWebSocket();\
    initButton();\
  }\
  function initButton() {\
    document.getElementById('button').addEventListener('click', toggle);\
  }\
  function toggle(){\
    websocket.send('toggle');\
  }\
</script>\
</body>\
</html>\
";

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

const char *SSID_ = "ssid";
const char *PASS_ = "pass";
void WebInit()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", HTML); });

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

AsyncWebSocket ws("/ws");
bool ledState = 0;
const int ledPin = 2;

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