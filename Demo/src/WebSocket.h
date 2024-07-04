#include <Arduino.h>
#include "WebServer.h"

bool ledState = 0;
const int ledPin = 25;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
void accessPointMode(){
  WiFi.mode(WIFI_AP);
  Serial.println("Configuring access point!");
  WiFi.softAP(ssid_AP,password_AP);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
}//accessPointMode
void Switch(){
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    timeout++;
    Serial.print(".");
    if(timeout == 10){
      accessPointMode();
      timeout = 0;
      break;
    }
  }
}//Switch
void stationMode(){
  Serial.println();
  Serial.println();
  loadSetting();
  Serial.print("Connecting to ");
  Serial.println(settings.ssid);
  unsigned long start = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(settings.ssid,settings.pass);
  if(WiFi.status() != WL_CONNECTED)
  {
    Switch();
  }
  else {
    Serial.println(WiFi.localIP());
  }
    Serial.println(WiFi.localIP());
    server.begin();
}//stationMode
String ws_load = "";
void notifyClients(String value) {
  ws.textAll(value);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if(strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      notifyClients(String(ledState));
    }
    else{ 
      String DataStr = "";
      for(int i = 0 ; i < len ; i++){DataStr += (char)data[i];}
      LOGLN("Data: " + DataStr);
      JsonDocument doc;
      deserializeJson(doc, DataStr);
      String SSID = doc["SSID"].as<String>();
      String PASS = doc["PASS"].as<String>();
      String STATIC_IP = doc["STATIC_IP"].as<String>(); 
      String WEB_ADDRESS = doc["WEB_ADDRESS"].as<String>(); 
      String WEB_SUBNET = doc["WEB_SUBNET"].as<String>(); 
      String WEB_GETWAY = doc["WEB_GETWAY"].as<String>(); 
      String WIFI_MODE = doc["WIFI_MODE"].as<String>(); 
      String CHANEL = doc["CHANEL"].as<String>(); 
      if(STATIC_IP == "0") STATIC_IP = "Disable";
      else if(STATIC_IP == "1") STATIC_IP = "Enable";
      if(WIFI_MODE == "0") WIFI_MODE = "STA NORMAL";
      if(WIFI_MODE == "1") WIFI_MODE = "AP NORMAL";
      if(WIFI_MODE == "2") WIFI_MODE = "AP-STA NORMAL";
      if(WIFI_MODE == "3") WIFI_MODE = "WIFI OFF";
      LOGLN("SSID: " + SSID);
      LOGLN("STATIC_IP: "+ STATIC_IP); 
      LOGLN("WEB_SUBNET: "+ WEB_SUBNET); 
      LOGLN("WEB_ADDRESS: "+ WEB_ADDRESS); 
      LOGLN("WEB_GETWAY: "+ WEB_GETWAY); 
      LOGLN("WIFI_MODE: "+ WIFI_MODE); 
      LOGLN("CHANEL: "+ CHANEL); 

      settings.ssid = SSID;
      settings.pass = PASS;
      settings.staticip = STATIC_IP;
      settings.waddress = WEB_ADDRESS;
      settings.wsubnet = WEB_SUBNET;
      settings.wgetway = WEB_GETWAY;
      settings.wifimode = WIFI_MODE;
      settings.chanel = CHANEL;
      writeSetting();
      stationMode();

    }
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