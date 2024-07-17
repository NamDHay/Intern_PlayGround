#include <OnlineManage.h>
#include <MB.h>
#include <HTML.h>
#include <File_System.h>
#include <IO_Config.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

Website mywebsite;

const IPAddress local_IP(192, 168, 1, 128);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 0, 0);

const IPAddress localIP(192, 168, 4, 1);      // the IP address the web server, Samsung requires the IP to be in public space
const IPAddress gatewayIP(192, 168, 4, 1);    // IP address of the network should be the same as the local IP for captive portals
const IPAddress subnetMask(255, 255, 255, 0); // no need to change: https://avinetworks.com/glossary/subnet-mask/

const String localIPURL = "http://192.168.4.1/setting/"; // a string version of the local IP with http, used for redirecting clients to your webpage

const char *ntpServer = "vn.pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

void OnlineManage::Init()
{
    online.loadSetting();
    Serial.print("Connecting to ");
    Serial.println(online.wifi_setting.ssid);
    online.AP_STA_Mode();
    if (!WiFi.config(local_IP, gateway, subnet))
    {
        Serial.println("STA Failed to configure");
    }
    if (online.CheckConnect(500))
    {
        Serial.print("Connected!\n");
        online.Get_STA_IP();
    }
    else
    {
        Serial.println("Connection failed\n");
        Serial.println("Change to AP_Mode\n");
        online.AP_Mode();
        online.Get_AP_IP();
    }
    online.NTPInit();
    online.WebSocketInit();
    online.PortalInit();
    online.WebHandle();
    server.begin();
}
void OnlineManage::AP_Mode()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
}
void OnlineManage::STA_Mode()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(online.wifi_setting.ssid, online.wifi_setting.pass);
}
void OnlineManage::Get_STA_IP()
{
    Serial.print("STA -> IP address: ");
    Serial.println(WiFi.localIP());
}
void OnlineManage::Get_AP_IP()
{
    Serial.print("AP -> IP address: ");
    Serial.println(WiFi.softAPIP());
}
void OnlineManage::AP_STA_Mode()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
    WiFi.begin(online.wifi_setting.ssid, online.wifi_setting.pass);
}
bool OnlineManage::CheckConnect(uint32_t timeout)
{
    static long startTime = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        if (timeout == 0)
        {
            return false;
        }
        if (millis() - startTime >= 10)
        {
            startTime = millis();
            timeout--;
        }
    }
    return true;
}

void OnlineManage::loadSetting()
{
    String dataRead = "";
    JsonDocument doc;
    dataRead = filesystem.readfile("/wfsetting.json");
    deserializeJson(doc, dataRead);

    String SSID = doc["SSID"].as<String>();
    String PASS = doc["PASS"].as<String>();
    String waddress = doc["waddress"].as<String>();
    String wgetway = doc["wgetway"].as<String>();
    String wsubnet = doc["wsubnet"].as<String>();
    String staip = doc["staip"].as<String>();
    String wmode = doc["wmode"].as<String>();

    Serial.println("SSID: " + SSID);
    Serial.println("PASS: " + PASS);
    Serial.println("waddress: " + waddress);
    Serial.println("wgetway: " + wgetway);
    Serial.println("wsubnet: " + wsubnet);
    Serial.println("staip: " + staip);
    Serial.println("wmode: " + wmode);

    online.wifi_setting.ssid = SSID;
    online.wifi_setting.pass = PASS;
    online.wifi_setting.waddress = waddress;
    online.wifi_setting.wgetway = wgetway;
    online.wifi_setting.wsubnet = wsubnet;
    online.wifi_setting.staip = staip;
    online.wifi_setting.wmode = wmode;
}
void OnlineManage::writeSetting()
{
    String setting = "";
    JsonDocument writeDoc;

    writeDoc["SSID"] = online.wifi_setting.ssid;
    writeDoc["PASS"] = online.wifi_setting.pass;
    writeDoc["waddress"] = online.wifi_setting.waddress;
    writeDoc["wgetway"] = online.wifi_setting.wgetway;
    writeDoc["wsubnet"] = online.wifi_setting.wsubnet;
    writeDoc["staip"] = online.wifi_setting.staip;
    writeDoc["wmode"] = online.wifi_setting.wmode;

    serializeJson(writeDoc, setting);
    Serial.println("JSON: " + setting);
    filesystem.writefile("/wfsetting.json", setting, 0);
}

#define DNS_INTERVAL 30
DNSServer dnsServer;
void OnlineManage::PortalInit()
{
    dnsServer.setTTL(3600);
    dnsServer.start(53, "*", WiFi.softAPIP());

    Serial.print("\n");
    Serial.print("Startup Time:"); // should be somewhere between 270-350 for Generic ESP32 (D0WDQ6 chip, can have a higher startup time on first boot)
    Serial.println(millis());
    Serial.print("\n");
}

void OnlineManage::notifyClients(const String &message)
{
    ws.textAll(message);
}

JsonDocument rdoc;
JsonDocument wDoc;
String DataStr = "";
String fbDataString = "";
void getIOHandler()
{
    int DataArr[10];
    int size = sizeof(DataArr);
    for (int i = 0; i < 10; i++)
    {
        DataArr[i] = rdoc["Data"][i];
    }
    Serial.flush();
}
void mbDataTypeHandler()
{
    size_t size = rdoc["lengh"];
    Serial.println("Size of type: " + String(size));
    for (int i = 0; i < size; i++)
    {
        modbusRTU.typeData[i] = rdoc["type"][i];
        Serial.println("Type " + String(i) + ": " + String(modbusRTU.typeData[i]));
    }
    bool IsSetTable = true;
    xQueueSend(modbusRTU.qUpdateTable, (void *)&IsSetTable, 1 / portTICK_PERIOD_MS);
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

    modbusRTU.config.slaveID = slaveID.toInt();
    modbusRTU.master = (mbmaster == "0") ? 0 : 1;
    modbusRTU.config.baud = baud.toInt();
    modbusRTU.config.port = (serial == "0") ? &Serial1 : &Serial2;
    if (modbusRTU.master == 1)
    {
        modbusRTU.MasterReadReg.startAddress = readStart.toInt();
        modbusRTU.MasterReadReg.endAddress = readEnd.toInt();
        modbusRTU.MasterWriteReg.startAddress = writeStart.toInt();
        modbusRTU.MasterWriteReg.endAddress = writeEnd.toInt();
    }

    if (modbusRTU.master == 0)
    {
        modbusRTU.SlaveReadReg.startAddress = readStart.toInt();
        modbusRTU.SlaveReadReg.endAddress = readEnd.toInt();
        modbusRTU.SlaveWriteReg.startAddress = writeStart.toInt();
        modbusRTU.SlaveWriteReg.endAddress = writeEnd.toInt();
    }
    wDoc["Command"] = "settingModbus";
    wDoc["Data"] = "SetingDone";
    serializeJson(wDoc, fbDataString);
    online.notifyClients(fbDataString);
    modbusRTU.writeSetting();
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

    Serial.println("SSID: " + SSID);
    Serial.println("PASS: " + PASS);
    Serial.println("waddress: " + waddress);
    Serial.println("wgetway: " + wgetway);
    Serial.println("wsubnet: " + wsubnet);
    Serial.println("staip: " + staip);
    Serial.println("wmode: " + wmode);

    online.wifi_setting.ssid = SSID;
    online.wifi_setting.pass = PASS;
    online.wifi_setting.waddress = waddress;
    online.wifi_setting.wgetway = wgetway;
    online.wifi_setting.wsubnet = wsubnet;
    online.wifi_setting.staip = staip;
    online.wifi_setting.wmode = wmode;
    online.writeSetting();
    wDoc["Command"] = "settingWifi";
    wDoc["Data"] = "SetingDone";
    serializeJson(wDoc, fbDataString);
    online.notifyClients(fbDataString);
    bool IsMessage = true;
    xQueueSend(online.qWifiSetting, (void *)&IsMessage, 1 / portTICK_PERIOD_MS);
}
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
        String command = rdoc["Command"].as<String>();
        if (command == "toggleLed")
        {
            wDoc["Command"] = "toggleLed";
            wDoc["Data"] = digitalRead(LED);
            serializeJson(wDoc, fbDataString);
            online.notifyClients(fbDataString);
            digitalWrite(LED, !digitalRead(LED));
        }
        else if (command == "getMem")
        {
            Serial.print("Usage heap memory: ");
            Serial.println((ESP.getHeapSize() - ESP.getFreeHeap()));
            wDoc["Command"] = "getMem";
            wDoc[""] = (ESP.getHeapSize() - ESP.getFreeHeap());
            serializeJson(wDoc, fbDataString);
            online.notifyClients(fbDataString);
        }
        else if (command == "getIO")
        {
            // getIOHandler();
        }
        else if (command == "settingModbus")
        {
            filesystem.writefile("/mbsetting.json", DataStr, 0);
            setModbusHandler();
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
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        online.isConnected = true;
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        online.isConnected = false;
        modbusRTU.loadTable = true;
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    }
}
void OnlineManage::WebSocketInit()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

const char *SSID_ = "ssid";
const char *PASS_ = "pass";
void OnlineManage::WebHandle()
{
    server.on("/connecttest.txt", [](AsyncWebServerRequest *request)
              { request->redirect("http://logout.net"); }); // windows 11 captive portal workaround
    server.on("/wpad.dat", [](AsyncWebServerRequest *request)
              { request->send(404); }); // Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

    server.on("/generate_204", [](AsyncWebServerRequest *request)
              { request->redirect(localIPURL); }); // android captive portal redirect
    server.on("/redirect", [](AsyncWebServerRequest *request)
              { request->redirect(localIPURL); }); // microsoft redirect
    server.on("/hotspot-detect.html", [](AsyncWebServerRequest *request)
              { request->redirect(localIPURL); }); // apple call home
    server.on("/canonical.html", [](AsyncWebServerRequest *request)
              { request->redirect(localIPURL); }); // firefox captive portal call home
    server.on("/success.txt", [](AsyncWebServerRequest *request)
              { request->send(200); }); // firefox captive portal call home
    server.on("/chrome-variations/seed", [](AsyncWebServerRequest *request)
              { request->send(200); }); // chrome captive portal call home
    server.on("/ncsi.txt", [](AsyncWebServerRequest *request)
              { request->redirect(localIPURL); }); // windows call home

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });
    // server.serveStatic("/", SPIFFS, "/");
    server.on("/setting", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html", mywebsite.Wifi_Config_HTML);
        request->send(response);
        Serial.println("Served Wifi Config Page"); });
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String Ssid;
    String Pass;
    if (request->hasParam(SSID_)) {
        Ssid = request->getParam(SSID_)->value();
    } else {
        Ssid = "khong co SSID";
    }
    online.wifi_setting.ssid = Ssid;
    if (request->hasParam(PASS_)) {
        Pass = request->getParam(PASS_)->value();
    } else {
        Pass = "khong co SSID";
    }
    online.wifi_setting.pass = Pass;
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", mywebsite.Wifi_Config_HTML);
    request->send(response);
    bool setWifi = true;
    xQueueSend(online.qPortalSetting, (void*) &setWifi, 1/portTICK_PERIOD_MS); });
}

void OnlineManage::NTPInit()
{
    Serial.print("Go Here");
    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "asia.pool.ntp.org");
    return;
}
void OnlineManage::printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    Serial.print("Day of week: ");
    Serial.println(&timeinfo, "%A");
    Serial.print("Month: ");
    Serial.println(&timeinfo, "%B");
    Serial.print("Day of Month: ");
    Serial.println(&timeinfo, "%d");
    Serial.print("Year: ");
    Serial.println(&timeinfo, "%Y");
    Serial.print("Hour: ");
    Serial.println(&timeinfo, "%H");
    Serial.print("Hour (12 hour format): ");
    Serial.println(&timeinfo, "%I");
    Serial.print("Minute: ");
    Serial.println(&timeinfo, "%M");
    Serial.print("Second: ");
    Serial.println(&timeinfo, "%S");
    Serial.println("Time variables");
    char timeHour[3];
    strftime(timeHour, 3, "%H", &timeinfo);
    Serial.println(timeHour);
    char timeWeekDay[10];
    strftime(timeWeekDay, 10, "%A", &timeinfo);
    Serial.println(timeWeekDay);
    Serial.println();
}

void TaskOnlineManager(void *pvParameter)
{

    static long startCheckWifiTime = millis();
    static bool setWifi = false;
    static bool isMessageReceived = false;
    online.qPortalSetting = xQueueCreate(1, sizeof(bool));
    online.qWifiSetting = xQueueCreate(1, sizeof(bool));
    for (;;)
    {
        if (xQueueReceive(online.qPortalSetting, (void *)&setWifi, 1 / portTICK_PERIOD_MS) == pdTRUE)
        {
            WiFi.begin(online.wifi_setting.ssid, online.wifi_setting.pass);
            online.writeSetting();
            setWifi = false;
            Serial.print("Im in queue Portal Send seting\n");
        }
        if (xQueueReceive(online.qWifiSetting, (void *)&isMessageReceived, 1 / portTICK_PERIOD_MS) == pdTRUE)
        {
            WiFi.begin(online.wifi_setting.ssid, online.wifi_setting.pass);
            isMessageReceived = false;
        }
        if (millis() - startCheckWifiTime >= WIFI_STATUS_INTERVAL)
        {
            startCheckWifiTime = millis();
            if (WiFi.status() != WL_CONNECTED)
            {
                // Serial.println("Wifi Disconnected");
            }
        }
        dnsServer.processNextRequest();
        ws.cleanupClients();
    }
}