
#define LOG(x) {Serial.print(x);}
#define LOGLN(x) {Serial.println(x);}

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>


struct Settings
{
  String ssid;
  String pass;
}settings;



#include "Filesystem.h"
#include "fileproccess.h"
// #define example



// STA Mode
const char* ssid_STA = "iSoft";
const char* password_STA = "i-soft@123";
// AP Mode
const char* ssid_AP = "NODE_IOT";
const char* password_AP = "123456789";

short timeout = 0;
bool runMode = false;

String header;
String HTML = "\
<!DOCTYPE html>\
<html>\
<body><CENTER>\
<h2>Cai dat Wifi</h2>\
<form action=\"/get\">\
  <label for=\"fname\">ten wifi:</label><br>\
  <input type=\"text\" id=\"ssid\" name=\"ssid\" placeholder=\"ten wifi\"><br>\
  <label for=\"lname\">mat khau wifi:</label><br>\
  <input type=\"text\" id=\"pass\" name=\"pass\" placeholder=\"pass wifi\"><br><br>\
  <input type=\"submit\" value=\"Luu\">\
</form> </CENTER>\
</body>\
</html>\
";
const char* SSID_ = "ssid";
const char* PASS_ = "pass";

AsyncWebServer server(80);
void accessPointMode(){
  WiFi.mode(WIFI_AP);
  Serial.println("Configuring access point!");
  WiFi.softAP(ssid_AP,password_AP);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  runMode = true;

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
}
void stationMode(){
  Serial.println();
  Serial.println();
  loadSetting();
  Serial.print("Connecting to ");
  Serial.println(settings.ssid);
  unsigned long start = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(settings.ssid,settings.pass);
  // while(WiFi.status() != WL_CONNECTED && millis()-start < 10000){
  //   Serial.println(".");
  // }
  if(WiFi.status() != WL_CONNECTED)
  {
    runMode = true;
    Switch();
  }
  else {
    
    Serial.println(WiFi.localIP());
  }
  
    Serial.println(WiFi.localIP());
    server.begin();
}
void WebInit(){
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", HTML);
    });
    
    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
        ESP.restart();
        request->send(200, "text/plain", "reset done");
    });
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String Ssid;
        String Pass;
        if (request->hasParam(SSID_)) {
            Ssid = request->getParam(SSID_)->value();
        } else {
            Ssid = "khong co SSID";
        }
        settings.ssid = Ssid;
        if (request->hasParam(PASS_)) {
            Pass = request->getParam(PASS_)->value();
        } else {
            Pass = "khong co SSID";
        }
        settings.pass = Pass;
        
        request->send(200, "text/plain", "Hello, GET: " + Ssid + "|" + Pass);
        writeSetting();
    });
    
}
void setup() {
  Serial.begin(115200);
  SPIFFS_Init();
  stationMode();
  
  WebInit();
  #ifdef example
  fileproccess_example();
  #endif//example
}
void loop() {

}// loop()
