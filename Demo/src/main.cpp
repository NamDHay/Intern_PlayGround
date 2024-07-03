#define LOG(x) {Serial.print(x);}
#define LOGLN(x) {Serial.println(x);}

#include <Arduino.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "Filesystem.h"
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void TaskWiFi(void *pvParameter);
void TaskCaptivePortal(void *pvParameter);
void TaskFunction(void *pvParameter);
#define MAX_CLIENTS 4
#define WIFI_CHANNEL 6

const IPAddress localIP(192, 168, 4, 1);		
const IPAddress gatewayIP(192, 168, 4, 1);	   
const IPAddress subnetMask(255, 255, 255, 0);
const String localIPURL = "http://192.168.4.1";

struct Settings
{
  String ssid;
  String pass;
}settings;




#include "fileproccess.h"

// AP Mode
const char* ssid_AP = "NODE_IOT";
const char* password_AP = "123456789";

short timeout = 0;

DNSServer dnsServer;
// AsyncWebServer server(80);
#include "WebSocket.h"

String header;
String HTML = "\
<!DOCTYPE html>\
<html>\
<body><CENTER>\
<h1>WIFI SETTING</h1></CENTER>\
<form action=\"/get\">\
  <label for=\"fname\">SSID:</label>\
  <input type=\"text\" id=\"ssid\" name=\"ssid\" placeholder=\"Wifi Name\"><br><br>\
  <label for=\"lname\">PASSWORD:</label>\
  <input type=\"text\" id=\"pass\" name=\"pass\" placeholder=\"Wifi Pass\"><br><br>\
  <input type=\"submit\" value=\"SAVE\">\
  </div>\
</form>\
</body>\
</html>\
";
const char* SSID_ = "ssid";
const char* PASS_ = "pass";

void setUpDNSServer(DNSServer &dnsServer, const IPAddress &localIP) {
	dnsServer.setTTL(3600);
	dnsServer.start(53, "*", localIP);
}
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
        } 
        else {
          Ssid = "khong co SSID";
        }
        settings.ssid = Ssid;
        if (request->hasParam(PASS_)) {
          Pass = request->getParam(PASS_)->value();
        } 
        else {
          Pass = "khong co PASS";
        }
        settings.pass = Pass;
        
        request->send(200, "text/plain", Ssid + "|" + Pass);
        writeSetting();
    });
}//WebInit
void setUpWebserver(AsyncWebServer &server, const IPAddress &localIP) {
	
	// Required
	server.on("/connecttest.txt", [](AsyncWebServerRequest *request) { request->redirect("http://logout.net"); });	// windows 11 captive portal workaround
	server.on("/wpad.dat", [](AsyncWebServerRequest *request) { request->send(404); });								// Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

	// Background responses: Probably not all are Required, but some are. Others might speed things up?
	// A Tier (commonly used by modern systems)
	server.on("/generate_204", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });		   // android captive portal redirect
	server.on("/redirect", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // microsoft redirect
	server.on("/hotspot-detect.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });  // apple call home
	server.on("/canonical.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });	   // firefox captive portal call home
	server.on("/success.txt", [](AsyncWebServerRequest *request) { request->send(200); });					   // firefox captive portal call home
	server.on("/ncsi.txt", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // windows call home

	// return 404 to webpage icon
	server.on("/favicon.ico", [](AsyncWebServerRequest *request) { request->send(404); });	// webpage icon

	// Serve Basic HTML Page
	server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse(200, "text/html", HTML);
		response->addHeader("Cache-Control", "public,max-age=31536000");  // save this file to cache for 1 year (unless you refresh)
		request->send(response);
		Serial.println("Served Basic HTML Page");
	});

	// the catch all
	server.onNotFound([](AsyncWebServerRequest *request) {
		request->redirect(localIPURL);
		Serial.print("onnotfound ");
		Serial.print(request->host());	// This gives some insight into whatever was being requested on the serial monitor
		Serial.print(" ");
		Serial.print(request->url());
		Serial.print(" sent redirect to " + localIPURL + "\n");
	});
}

void setup() {
  Serial.begin(115200);
  SPIFFS_Init();
  stationMode();
  setUpDNSServer(dnsServer, WiFi.softAPIP());
  setUpWebserver(server, WiFi.softAPIP());
  WebInit();
  WB_setup();
  xTaskCreatePinnedToCore(TaskWiFi, "WiFi", 20000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskCaptivePortal, "CaptivePortal",2000,NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskFunction, "Function", 10000, NULL, 1,NULL, 1);
}
void loop(){}

void TaskWiFi(void *pvParameter){
  static long realtime = millis();
  static long Timedelay= 1;
  for(;;){
    if(millis()-realtime>Timedelay){realtime = millis();
    
    }  
  }
}
void TaskCaptivePortal(void *pvParameter){
  for(;;){
    dnsServer.processNextRequest();
    vTaskDelay(30/portTICK_PERIOD_MS);
  } 
}
void TaskFunction(void *pvParameter){
  for(;;){
    WB_loop();
    vTaskDelay(10/portTICK_PERIOD_MS);
  }
}