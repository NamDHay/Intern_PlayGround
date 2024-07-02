#include <File_System.h>
#include <Wifi_Config.h>

AsyncWebServer server(80);
QueueHandle_t qOnline;

struct wifi_setting_t {
  String ssid;
  String pass;
}wifisetting;

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
  <input type=\"password\" id=\"pass\" name=\"pass\" placeholder=\"pass wifi\"><br><br>\
  <input type=\"submit\" value=\"Save\">\
</form> </CENTER>\
</body>\
</html>\
";

void WifiConnect() {
  uint8_t timeout = 100;
  static long startTime = millis();
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(wifisetting.ssid);
  WiFi.begin(wifisetting.ssid, wifisetting.pass);
  while(WiFi.status() != WL_CONNECTED) {
    if(timeout == 0) {
      Serial.println("\nWiFi connected timeout\n");
      Serial.println("Change to AP_Mode\n");
      AP_Mode();
      return;
    }
    if(millis() - startTime >= 100){
      Serial.print(".");
      startTime = millis();
      timeout--;
    }
  }
  Serial.print("Connected!\n");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  return;
}
void AP_Mode() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
  return;
}

void loadSetting() {
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
void writeSetting() {
  String setting = "";
  JsonDocument writeDoc;
  writeDoc["SSID"] = wifisetting.ssid;
  writeDoc["PASS"] = wifisetting.pass;
  serializeJson(writeDoc, setting);
  Serial.println("JSON: "+ setting);
  writefile("/setting.json", setting, 0);
}

const char* SSID_ = "ssid";
const char* PASS_ = "pass";
void WebInit() {
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

void TaskOnlineManager(void *pvParameter) {;
  static bool SetWifi = false;
  qOnline = xQueueCreate(1, sizeof(bool));
  for(;;) {
    if(xQueueReceive(qOnline, (void*) &SetWifi, 1/portTICK_PERIOD_MS) == pdTRUE) {
      WifiConnect();
      writeSetting();
      SetWifi = false;
      Serial.print("Receive Queue Complete\n");
    }
  }
}