#include <Arduino.h>
#include "WebServer.h"

bool ledState = 0;
const int ledPin = 25;

JsonDocument rdoc;
JsonDocument wDoc;
String DataStr = "";
String fbDataString = "";

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
void getIOHandler()
{
    Serial.println("I'm here getIO");
    int DataArr[10];
    int size = sizeof(DataArr);
    for (int i = 0; i < 10; i++)
    {
        DataArr[i] = rdoc["Data"][i];
    }
    Serial.println("Data0: " + String(DataArr[0]));
    Serial.println("Data1: " + String(DataArr[1]));
    Serial.println("Data2: " + String(DataArr[2]));
    Serial.println("Data3: " + String(DataArr[3]));
    Serial.println("Data4: " + String(DataArr[4]));
    Serial.println("Data5: " + String(DataArr[5]));
    Serial.println("Data6: " + String(DataArr[6]));
    Serial.println("Data7: " + String(DataArr[7]));
    Serial.flush();
}
void setModbusHandler()
{
    String slaveID = rdoc["slaveID"].as<String>();
    String baud = rdoc["baud"].as<String>();
    String readStart = rdoc["readStart"].as<String>();
    String readEnd = rdoc["readEnd"].as<String>();
    String writeStart = rdoc["writeStart"].as<String>();
    String writeEnd = rdoc["writeEnd"].as<String>();
    String serial = rdoc["serial"].as<String>();
    String mbmaster = rdoc["mbmaster"].as<String>();

    mbusconfig.mode = (mbmaster == "0") ? true : false;
    mbusconfig.SlaveID_Config = slaveID.toInt();
    mbusconfig.baud = baud.toInt();

    wDoc["Command"] = "settingModbus";
    wDoc["Data"] = "SetingDone";
    serializeJson(wDoc, fbDataString);
    notifyClients(fbDataString);
}
void setWifiHandler()
{
    String SSID = rdoc["SSID"].as<String>();
    String PASS = rdoc["PASS"].as<String>();
    String waddress = rdoc["waddress"].as<String>();
    String wgetway = rdoc["wgetway"].as<String>();
    String wsubnet = rdoc["wsubnet"].as<String>();
    String staip = rdoc["staip"].as<String>();
    String wmode = rdoc["wmode"].as<String>();
    if(staip == "0") staip = "Disable";
    else if(staip == "1") staip = "Enable";
    if(wmode == "0") wmode = "STA NORMAL";
    else if(wmode == "1") wmode = "AP NORMAL";
    else if(wmode == "2") wmode = "AP-STA NORMAL";
    else if(wmode == "3") wmode = "WIFI OFF";
    Serial.println("SSID: " + SSID);
    Serial.println("PASS: " + PASS);
    Serial.println("waddress: " + waddress);
    Serial.println("wgetway: " + wgetway);
    Serial.println("wsubnet: " + wsubnet);
    Serial.println("staip: " + staip);
    Serial.println("wmode: " + wmode);

    settings.ssid = SSID;
    settings.pass = PASS;
    settings.waddress = waddress;
    settings.wgetway = wgetway;
    settings.wsubnet = wsubnet;
    settings.staticip = staip;
    settings.wifimode = wmode;
    writeSetting();
    wDoc["Command"] = "settingWifi";
    wDoc["Data"] = "SetingDone";
    serializeJson(wDoc, fbDataString);
    notifyClients(fbDataString);
    bool IsMessage = true;
    loadSetting();
}
void tabledata(){
  String json_data = " {\"Data\":[{\"value\":\"1\",\"type\":\"0\",\"slaveID\":\"1\"},{\"value\":\"2\",\"type\":\"3\",\"slaveID\":\"10\"},{\"value\":\"3\",\"type\":\"2\",\"slaveID\":\"3\"},{\"value\":\"4\",\"type\":\"1\",\"slaveID\":\"5\"},{\"value\":\"5\",\"type\":\"3\",\"slaveID\":\"133\"},{\"value\":\"6\",\"type\":\"1\",\"slaveID\":\"44\"},{\"value\":\"7\",\"type\":\"0\",\"slaveID\":\"20\"}]}";
  wDoc["Command"] = "Data";
  serializeJson(wDoc,json_data);
  notifyClients(json_data);
}
//Receive data from websocket
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        DataStr = "";
        for (int i = 0; i < len; i++)
        {
            DataStr += (char)data[i];
        }
        deserializeJson(rdoc, DataStr);
        Serial.println(DataStr);
        String command = rdoc["Command"].as<String>();
        Serial.println(command);
        if (command == "toggleLed")
        {
            Serial.println("I'm here toggleLed");
            ledState = !ledState;
            wDoc["Command"] = "toggleLed";
            wDoc["Data"] = ledState;
            digitalWrite(ledPin, ledState);
            serializeJson(wDoc, fbDataString);
            notifyClients(fbDataString);
        }
        else if (command == "getIO")
        {
            getIOHandler();
        }
        else if (command == "settingModbus")
        {
            setModbusHandler();
        }
        else if (command == "settingWifi")
        {
            setWifiHandler();
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