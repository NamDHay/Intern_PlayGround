#include <Arduino.h>
#include "WebServer.h"
#include <ModbusConfig.h>
bool ledState = 0;
const int ledPin = 25;
bool FlagFile = false;
bool writeshowfile = false;
bool writename = false;
JsonDocument rdoc;
JsonDocument wDoc;
String DataStr = "";
String fbDataString = "";
static long startUpdateIntervalTime = millis();
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
//AP
void accessPointMode(){
  WiFi.mode(WIFI_AP);
  Serial.println("Configuring access point!");
  WiFi.softAP(ssid_AP,password_AP);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
}//accessPointMode
//Switch
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
//STA
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
//Send data to web
void notifyClients(const String &value) {
  ws.textAll(value);
}
//getIO
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
//setupModbus
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

    mbusconfig.slaveID = slaveID.toInt();
    master = (mbmaster == "0") ? 0 : 1;
    mbusconfig.baud = baud.toInt();
    mbusconfig.port = (serial == "0") ? &Serial1 : &Serial2;
    if (master == 1)
    {
        MasterReadReg.startAddress = readStart.toInt();
        MasterReadReg.endAddress = readEnd.toInt();
        MasterWriteReg.startAddress = writeStart.toInt();
        MasterWriteReg.endAddress = writeEnd.toInt();
    }

    if (master == 0)
    {
        SlaveReadReg.startAddress = readStart.toInt();
        SlaveReadReg.endAddress = readEnd.toInt();
        SlaveWriteReg.startAddress = writeStart.toInt();
        SlaveWriteReg.endAddress = writeEnd.toInt();
    }

    wDoc["Command"] = "settingModbus";
    wDoc["Data"] = "SetingDone";
    serializeJson(wDoc, fbDataString);
    notifyClients(fbDataString);
    Modbus_writeSetting();
}
//updateWeb
void update_WebData_Interval() // Must be void function to avoid control reaches end of non-void function [-Wreturn-type]
{
  union f2w_t
  {
    float f;
    int16_t w[2];
  } f2w;
  union dw2w_t
  {
    int32_t dw;
    int16_t w[2];
  } dw2w;
  JsonDocument Doc; // this guy make error messages if not return void
  String fbDataString = "";
  long Address;
  if (isConnected == true)
  {
    Doc["Command"] = "Data";
    Address = MasterReadReg.startAddress;
    for (uint8_t i = 0; i < (MasterReadReg.endAddress - MasterReadReg.startAddress + 1); i++)
    {
      Doc["Data"][i]["slaveID"] = mbusconfig.slaveID;
      if (Address > MasterReadReg.endAddress)
      {
        Serial.println("Out of range");
        break;
      }
      Doc["Data"][i]["address"] = Address;
      Doc["Data"][i]["type"] = typeData[i];
      switch (typeData[i])
      {
      case COIL_TYPE:
        Doc["Data"][i]["value"] = CHECKCOIL(Master_ReadData[Address], 0);
        Address++;
        break;
      case WORD_TYPE:
        Doc["Data"][i]["value"] = Master_ReadData[Address];
        Address += 1;
        break;
      case DWORDS_TYPE:
        dw2w.w[0] = Master_ReadData[Address];
        dw2w.w[1] = Master_ReadData[Address + 1];
        Doc["Data"][i]["value"] = dw2w.dw;
        Address += 2;
        break;
      case FLOAT_TYPE:
        f2w.w[0] = Master_ReadData[Address];
        f2w.w[1] = Master_ReadData[Address + 1];
        Doc["Data"][i]["value"] = f2w.f;
        Address += 2;
        break;
      default:
        break;
      }
    }
    serializeJson(Doc, fbDataString);
    notifyClients(fbDataString);
  }
}
//ModbusDataType
void mbDataTypeHandler()
{
    size_t size = rdoc["lengh"];
    Serial.println("Size of type: " + String(size));
    for (int i = 0; i < size; i++)
    {
        typeData[i] = rdoc["type"][i];
        Serial.println("Type " + String(i) + ": " + String(typeData[i]));
    }
}
//setwifi
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
    // loadSetting();
    stationMode();
}
//showfile
void showfile(){

  JsonDocument fileDoc;
  JsonArray dataArray;
  JsonDocument datafile;
  String showfile = "";
  fileDoc["Command"] = "ShowFile";
  dataArray = fileDoc["ShowFile"].to<JsonArray>();

  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file){
    datafile["slaveID"] = mbusconfig.slaveID;
    datafile["type"] = random(0,2);
    String name = file.name(); 
    datafile["name"] = name; 
    datafile["space"] = file.size();
    datafile["total"] = SPIFFS.totalBytes();
    dataArray.add(datafile);
    file = root.openNextFile();
    writeshowfile = false;
  }
  
  serializeJson(fileDoc,showfile);
  notifyClients(showfile);
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
      Modbus_loadSetting();
    }
    else if (command == "settingWifi")
    {
      setWifiHandler();
    }
    else if (command == "mbDataType")
    {
        mbDataTypeHandler();
    }
  }
}
//onEvent
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      FlagFile = true;
      isConnected = true;
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
//initwebsocket
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
  if(FlagFile == true){
    showfile();
    Modbus_loadSetting();
    FlagFile = false;
  }
  // Modbus send data
  if (millis() - startUpdateIntervalTime >= 5000)
  {
    startUpdateIntervalTime = millis();
    update_WebData_Interval();
  }
}
void TaskFunction(void *pvParameter){
  
  for(;;){
    WB_loop();
    // vTaskDelay(1000/portTICK_PERIOD_MS);
  }
} 