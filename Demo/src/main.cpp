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

void TaskCaptivePortal(void *pvParameter);
void TaskFunction(void *pvParameter);

FileSystem fsFunc;

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

#include "WebServer.h"
WebServer webFunc;
// AP Mode
const char* ssid_AP = "NODE_IOT";
const char* password_AP = "123456789";

short timeout = 0;

DNSServer dnsServer;
#include "WebSocket.h"

String header;
String HTML = "\
<!DOCTYPE HTML>\
<html>\
<head>\
  <title>WIFI SETTING</title>\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  <link rel=\"icon\" href=\"data:,\">\
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
  <title>WIFI SETTING</title>\
<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
<link rel=\"icon\" href=\"data:,\">\
</head>\
<body>\
  <div class=\"topnav\">\
    <h1>WIFI SETTING</h1>\
  </div>\
  <div class=\"content\">\
    <div class=\"card\">\
    <span><label for=\"fname\">SSID:</label>\
    <input type=\"text\" id=\"input_ssid\" placeholder=\"Wifi Name\"></span><br><br>\
    <span><label for=\"lname\">PASSWORD:</label>\
    <input type=\"text\" id=\"input_pass\" placeholder=\"Wifi Pass\"></span><br><br>\
    <span><button id=\"buttonsave\" class=\"button\">SAVE</button></span>\
    </div>\
  </div>\
  <div class=\"content\">\
    <div class=\"card\">\
      <h2>LED STATUS</h2>\
      <p class=\"state\">STATE: <span id=\"state\">STATE</span></p>\
      <p><button id=\"buttontoggle\" class=\"button\">TOGGLE</button></p>\
    </div>\
  </div>\
<script>\
  var gateway = `ws://192.168.4.1/ws`;\
  var websocket;\
  function initWebSocket() {\
    console.log('Trying to open a WebSocket connection...');\
    websocket = new WebSocket(gateway);\
    websocket.onopen    = onOpen;\
    websocket.onclose   = onClose;\
    websocket.onmessage = onMessage; \
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
    if (event.data == \"1\"){\
      state = \"ON\";\
    }\
    else{\
      state = \"OFF\";\
    }\
    document.getElementById('state').innerHTML = state;\
  }\
  window.addEventListener('load', onLoad);\
  function onLoad(event) {\
    initWebSocket();\
    initButton();\
  }\
  function initButton() {\
    document.getElementById('buttontoggle').addEventListener('click', toggle);\
    document.getElementById('buttonsave').addEventListener('click', save);\
  }\
  function toggle(){\
    websocket.send('toggle');\
  }\
  function save(){\
    var ssid_input = document.getElementById('input_ssid').value;\
    var pass_input = document.getElementById('input_pass').value;\
    if(ssid_input == \"\" && pass_input == \"\"){\
      console.log(\"Fail SSID và PASS\");\
      alert(\"Fail SSID và PASS\");\
    }\
    else if(ssid_input == \"\"){\
      console.log(\"Pass SSID\");\
      alert(\"Pass SSID\");\
    }\
    else if(pass_input == \"\"){\
      console.log(\"Pass Fail\");\
      alert(\"Pass Fail\");\
    }\
    else{\
      var json_output = \"{'SSID':'\" + ssid_input + \"'','PASS':'\" + pass_input +\"'}\";\
      console.log(json_output);\
      websocket.send(json_output);\
      alert(\"Completed setting!!!\");\
    }\
  }\
</script>\
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
  webFunc.loadSetting();
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
        webFunc.writeSetting();
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
  fsFunc.SPIFFS_Init();
  stationMode();
  setUpDNSServer(dnsServer, WiFi.softAPIP());
  setUpWebserver(server, WiFi.softAPIP());
  WebInit();
  WB_setup();
  xTaskCreatePinnedToCore(TaskCaptivePortal, "CaptivePortal",2000,NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskFunction, "Function", 10000, NULL, 1,NULL, 1);
}
void loop(){}

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