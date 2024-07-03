#include <Arduino.h>
Function FuncSocket;
bool ledState = 0;
const int ledPin = 25;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
String ws_load = "";
void notifyClients(String value) {
  ws.textAll(value);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String DataStr = "";
    for(int i = 0 ; i < len ; i++){DataStr += (char)data[i];}
    LOGLN("Data: " + DataStr);
    JsonDocument doc;//
    deserializeJson(doc, DataStr);
    String SSID = doc["SSID"].as<String>();
    String PASS = doc["PASS"].as<String>();
    LOGLN("SSID: " + SSID);
    LOGLN("PASS: "+ PASS); 
    settings.ssid = SSID;
    settings.pass = PASS;
    FuncSocket.writeSetting();
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
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

void WB_setup(){
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);      
  initWebSocket();
}
void WB_loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
}