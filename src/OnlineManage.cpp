#include <OnlineManage.h>
#include <MB.h>
#include <HTML.h>
#include <File_System.h>
#include <time.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

Website mywebsite;

const IPAddress local_IP(192, 168, 1, 128);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 0, 0);

const IPAddress localIP(192, 168, 4, 1);      // the IP address the web server, Samsung requires the IP to be in public space
const IPAddress gatewayIP(192, 168, 4, 1);    // IP address of the network should be the same as the local IP for captive portals
const IPAddress subnetMask(255, 255, 255, 0); // no need to change: https://avinetworks.com/glossary/subnet-mask/

const String localIPURL = "http://192.168.4.1/"; // a string version of the local IP with http, used for redirecting clients to your webpage

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
    Serial.print("Startup Time:");
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
// {"Command":"SlaveArray","SlaveArray":[{"slaveType":"1","ID":"undefined","writeStart":"9120","writeEnd":"9129","readStart":"6096","readEnd":"9119"}]}
void mbSlavehandler()
{
    mbParam.numSlave++;

    mbParam.slave[(mbParam.numSlave - 1)].ID = rdoc["SlaveArray"][0]["ID"].as<String>();
    mbParam.slave[(mbParam.numSlave - 1)].ReadAddress.startAddress = rdoc["SlaveArray"][0]["readStart"];
    mbParam.slave[(mbParam.numSlave - 1)].ReadAddress.endAddress = rdoc["SlaveArray"][0]["readEnd"];
    mbParam.slave[(mbParam.numSlave - 1)].WriteAddress.startAddress = rdoc["SlaveArray"][0]["writeStart"];
    mbParam.slave[(mbParam.numSlave - 1)].WriteAddress.endAddress = rdoc["SlaveArray"][0]["writeEnd"];

    Serial.println("Node: " + String((mbParam.numSlave - 1)));
    Serial.println("ID: " + String(mbParam.slave[(mbParam.numSlave - 1)].ID));
    Serial.println("ReadStart: " + String(mbParam.slave[(mbParam.numSlave - 1)].ReadAddress.startAddress));
    Serial.println("ReadEnd: " + String(mbParam.slave[(mbParam.numSlave - 1)].ReadAddress.endAddress));
    Serial.println("WriteStart: " + String(mbParam.slave[(mbParam.numSlave - 1)].WriteAddress.startAddress));
    Serial.println("WriteEnd: " + String(mbParam.slave[(mbParam.numSlave - 1)].WriteAddress.endAddress));

    mbParam.writeSlave();
    wDoc["Command"] = "SlaveArray";
    wDoc["Data"] = "AddDone";
    serializeJson(wDoc, fbDataString);
    online.notifyClients(fbDataString);
}
void mbSendSlavehandler()
{
    wDoc["Command"] = "getTotalSlave";
    for (byte i = 0; i < mbParam.numSlave; i++)
    {
        wDoc["SlaveArray"][i]["ID"] = mbParam.slave[i].ID;
        wDoc["SlaveArray"][i]["readStart"] = mbParam.slave[i].ReadAddress.startAddress;
        wDoc["SlaveArray"][i]["readEnd"] = mbParam.slave[i].ReadAddress.endAddress;
        wDoc["SlaveArray"][i]["writeStart"] = mbParam.slave[i].WriteAddress.startAddress;
        wDoc["SlaveArray"][i]["writeEnd"] = mbParam.slave[i].WriteAddress.endAddress;
    }

    serializeJson(wDoc, fbDataString);
    Serial.println(fbDataString);
    online.notifyClients(fbDataString);
}
void setModbusHandler()
{
    String modbustype = rdoc["modbustype"].as<String>();

    String baud;
    String serial;
    String mbmaster;

    String ethip;
    String gw;
    String sn;
    String dns;
    String mbclient;

    if (modbustype == "0")
    {
        modbusRTU.master = 1;
        modbusTCP.client = 0;

        baud = rdoc["baud"].as<String>();
        serial = rdoc["serial"].as<String>();
        mbmaster = rdoc["mbmaster"].as<String>();

        // modbusRTU.master = (mbmaster == "0") ? 0 : 1;
        modbusRTU.config.baud = baud.toInt();
        modbusRTU.config.port = (serial == "0") ? &Serial1 : &Serial2;
    }
    else if (modbustype == "1")
    {
        modbusTCP.client = 1;
        modbusRTU.master = 0;

        ethip = rdoc["ethip"].as<String>();
        gw = rdoc["gw"].as<String>();
        sn = rdoc["sn"].as<String>();
        dns = rdoc["dns"].as<String>();
        mbclient = rdoc["mbclient"].as<String>();

        modbusTCP.ethernet.ipAdress = ethip;
        modbusTCP.ethernet.gateway = gw;
        modbusTCP.ethernet.subnet = sn;
        modbusTCP.ethernet.primaryDNS = dns;
    }
    else
    {
        modbusTCP.client = 1;
        modbusRTU.master = 1;

        baud = rdoc["baud"].as<String>();
        serial = rdoc["serial"].as<String>();
        mbmaster = rdoc["mbmaster"].as<String>();

        modbusRTU.config.baud = baud.toInt();
        modbusRTU.config.port = (serial == "0") ? &Serial1 : &Serial2;

        ethip = rdoc["ethip"].as<String>();
        gw = rdoc["gw"].as<String>();
        sn = rdoc["sn"].as<String>();
        dns = rdoc["dns"].as<String>();
        mbclient = rdoc["mbclient"].as<String>();

        modbusTCP.ethernet.ipAdress = ethip;
        modbusTCP.ethernet.gateway = gw;
        modbusTCP.ethernet.subnet = sn;
        modbusTCP.ethernet.primaryDNS = dns;
    }

    wDoc["Command"] = "settingModbus";
    wDoc["Data"] = "SetingDone";
    serializeJson(wDoc, fbDataString);
    online.notifyClients(fbDataString);
    modbusRTU.writeSetting();
    modbusTCP.writeSetting();
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
void editModbusData()
{
    // {"Command":"editModbusData","slaveID":"0","address":"6105","type":"0","value":"1"}
    uint8_t totalSend = 0;
    char *cstr;
    uint16_t *ustr;
    uint8_t node = rdoc["slaveID"];
    long address = rdoc["address"];
    uint8_t type = rdoc["type"];
    Serial.println("node: " + String(node) + " address: " + String(address) + " type: " + String(type));
    uint8_t stt = address - mbParam.slave[node].ReadAddress.startAddress;
    Serial.println("STT: " + String(stt));
    mbParam.slave->typeData[stt] = type;
    switch (type)
    {
    case COIL_TYPE:
        mbParam.write_data.w[0] = rdoc["value"];
        totalSend = 1;
        break;
    case WORD_TYPE:
        mbParam.write_data.w[0] = rdoc["value"];
        totalSend = 1;
        break;
    case DWORDS_TYPE:
        mbParam.write_data.dw = rdoc["value"];
        totalSend = 2;
        break;
    case FLOAT_TYPE:
        mbParam.write_data.f = rdoc["value"];
        totalSend = 2;
        break;
    case CHAR_TYPE:
        memset(cstr, 0, 40);
        memset(ustr, 0, 20);
        strcpy(cstr, rdoc["value"].as<String>().c_str());
        mbParam.c_to_u16(cstr, ustr);
        totalSend = 20;
        break;
    default:
        break;
    }
    if ((modbusRTU.master == 1) && (mbParam.slave[node].ID.length() < 5))
    {
        Serial.println("Write slave RTU");
        while (modbusRTU.write_Multiple_Data(mbParam.slave[node].ID.toInt(),
                                             (type == CHAR_TYPE) ? (uint16_t *)&ustr : (uint16_t *)&mbParam.write_data.w,
                                             address,
                                             totalSend) != true)
            ;
    }
    else if ((modbusTCP.client == 1) && (mbParam.slave[node].ID.length() > 5))
    {
        Serial.println("Write slave TCP");
        while (modbusTCP.write_Multiple_Data(modbusTCP.str2IP(mbParam.slave[node].ID),
                                             (uint16_t *)&mbParam.write_data.w,
                                             address,
                                             totalSend) != true)
            ;
    }
    bool IsSetTable = true;
    xQueueSend(mbParam.qUpdateTable, (void *)&IsSetTable, 1 / portTICK_PERIOD_MS);
    IsSetTable = false;
}
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    bool IsSetTable = true;
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
        if (command == "settingModbus")
        {
            // filesystem.writefile("/mbsetting.json", DataStr, 0);
            setModbusHandler();
        }
        else if (command == "settingWifi")
        {
            setWifiHandler();
        }
        else if (command == "SlaveArray") // Get from WebSocket and write to filesystem
        {
            mbSlavehandler();
        }
        else if (command == "getTotalSlave") // Request get total slave
        {
            mbSendSlavehandler();
        }
        else if (command == "clearSlave") // Clear slave in filesystem
        {
            filesystem.deletefile("/mbSlave.json");
            mbParam.loadSlave();
            mbSendSlavehandler();
        }
        else if (command == "loadSlaveTable") // Request load all slave tables to web server
        {
            IsSetTable = true;
            xQueueSend(mbParam.qUpdateTable, (void *)&IsSetTable, 1 / portTICK_PERIOD_MS);
            IsSetTable = false;
        }
        else if (command == "editModbusData") // Request change data from web server
        {
            editModbusData();
        }
        else if (command == "editModbusDataType") // Request change data from web server
        {
            uint8_t node = rdoc["slaveID"];
            long address = rdoc["address"];
            uint8_t type = rdoc["type"];
            uint8_t stt = address - mbParam.slave[node].ReadAddress.startAddress;
            mbParam.slave->typeData[stt] = type;
            IsSetTable = true;
            xQueueSend(mbParam.qUpdateTable, (void *)&IsSetTable, 1 / portTICK_PERIOD_MS);
            IsSetTable = false;
        }
        else if (command == "App") // Request application card information
        {
            filesystem.writefile("/application.json", DataStr, 0);
        }
        else if (command == "dataProduct") // Request product data
        {
            filesystem.writefile("/dataProduct.json", DataStr, 0);
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

    server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.html", "text/html");
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
        if (millis() - startCheckWifiTime > 0)
        {
            startCheckWifiTime = millis();
        }
        dnsServer.processNextRequest();
        ws.cleanupClients();
    }
}