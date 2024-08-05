#include <OnlineManage.h>
#include <MB.h>
#include <HTML.h>
#include <File_System.h>
#include <time.h>
#include <string.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

Website mywebsite;

const IPAddress local_IP(192, 168, 1, 254);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 0, 0);

void OnlineManage::Init()
{
    online.loadSetting();
    Serial.print("Connecting to ");
    Serial.println(online.wifi_setting.ssid);
    online.AP_STA_Mode();
    if (online.CheckConnect(500))
    {
        if (!WiFi.config(local_IP, gateway, subnet))
        {
            Serial.println("STA Failed to configure");
        }
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
    String ssid = "I-Soft";
    String pwk = "i-soft@2023";
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(soft_ap_ssid, soft_ap_password);
    // WiFi.begin(online.wifi_setting.ssid, online.wifi_setting.pass);
    WiFi.begin(ssid, pwk);
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

JsonDocument rdoc;
JsonDocument wDoc;
String DataStr = "";
String fbDataString = "";
void OnlineManage::notifyClients(const String &message)
{
    ws.textAll(message);
}
void mbAddSlavehandler()
{
    mbParam.numSlave++;

    mbParam.slave[(mbParam.numSlave - 1)].ID = rdoc["Slave"][0]["ID"].as<String>();
    mbParam.slave[(mbParam.numSlave - 1)].ReadAddress.startAddress = rdoc["Slave"][0]["readStart"];
    mbParam.slave[(mbParam.numSlave - 1)].ReadAddress.endAddress = rdoc["Slave"][0]["readEnd"];

    mbParam.writeSlave();
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
    modbusRTU.writeSetting();
    modbusTCP.writeSetting();
    modbusRTU.loadSetting();
    modbusTCP.loadSetting();
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
    online.loadSetting();
    bool IsMessage = true;
    xQueueSend(online.qWifiSetting, (void *)&IsMessage, 1 / portTICK_PERIOD_MS);
}
void editModbusData()
{
    uint8_t node = rdoc["slaveID"];
    long address = rdoc["address"];
    long length = rdoc["length"];
    uint16_t value[20];
    Serial.println("ID:" + mbParam.slave[node].ID + " | Address: " + String(address) + " | Length: " + String(length));
    for (size_t i = 0; i < length; i++)
    {
        value[i] = rdoc["value"][i];
        Serial.println("Value " + String(i) + " : " + String(value[i]));
    }
    mbParam.loadTable = false;
    if ((modbusRTU.master == 1) && (mbParam.slave[node].ID.length() < 5))
    {
        while (modbusRTU.write_Multiple_Data(mbParam.slave[node].ID.toInt(),
                                             (uint16_t *)&value,
                                             address,
                                             length) != true)
            ;
    }
    else if ((modbusTCP.client == 1) && (mbParam.slave[node].ID.length() > 5))
    {
        while (modbusTCP.write_Multiple_Data(modbusTCP.str2IP(mbParam.slave[node].ID),
                                             (uint16_t *)&value,
                                             address,
                                             length) != true)
            ;
        mbParam.loadTable = true;
    }
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
            setModbusHandler();
        }
        else if (command == "settingWifi")
        {
            setWifiHandler();
        }
        else if (command == "SlaveArray") // Add slave from WebSocket and write to filesystem
        {
            mbAddSlavehandler();
            mbParam.loadSlave();
        }
        else if (command == "editModbusData") // Request change data from web server
        {
            editModbusData();
        }
        else if (command == "SaveFile") // Request Save file
        {
            String filename = rdoc["Filename"].as<String>();
            String data = rdoc["Data"].as<String>();
            Serial.println(data);
            filesystem.writefile("/" + filename + ".json", data, 0);
        }
        else if (command == "LoadFile") // Request Load file
        {
            String filename = rdoc["Filename"].as<String>();
            String file = filesystem.readfile("/" + filename + ".json");
            fbDataString = "{\"Filename\":\"" + filename + "\",\"Data\":" + file + "}";
            Serial.println(fbDataString);
            online.notifyClients(fbDataString);
        }
        else if (command == "DeleteFile") // Request Delete file
        {
            String filename = rdoc["Filename"].as<String>();
            filesystem.deletefile("/" + filename + ".json");
        }
        else if (command == "ListFile") // Request List file
        {
            filesystem.ListFile();
        }
    }
}
void firstwebload()
{
    String file;
    filesystem.ListFile();

    file = filesystem.readfile("/mbSlave.json");
    Serial.println("{\"Filename\":\"mbSlave\",\"Data\":" + file + "}");
    online.notifyClients("{\"Filename\":\"mbSlave\",\"Data\":" + file + "}");

    file = filesystem.readfile("/TableID.json");
    Serial.println("{\"Filename\":\"TableID\",\"Data\":" + file + "}");
    online.notifyClients("{\"Filename\":\"TableID\",\"Data\":" + file + "}");

    mbParam.loadTable = true;

    file = filesystem.readfile("/Application.json");
    Serial.println("{\"Filename\":\"Application\",\"Data\":" + file + "}");
    online.notifyClients("{\"Filename\":\"Application\",\"Data\":" + file + "}");

    file = filesystem.readfile("/DataProduct.json");
    Serial.println("{\"Filename\":\"DataProduct\",\"Data\":" + file + "}");
    online.notifyClients("{\"Filename\":\"DataProduct\",\"Data\":" + file + "}");

    file = filesystem.readfile("/wfsetting.json");
    Serial.println("{\"Filename\":\"wfsetting\",\"Data\":" + file + "}");
    online.notifyClients("{\"Filename\":\"wfsetting\",\"Data\":" + file + "}");

    file = filesystem.readfile("/mbrtusetting.json");
    Serial.println("{\"Filename\":\"mbrtusetting\",\"Data\":" + file + "}");
    online.notifyClients("{\"Filename\":\"mbrtusetting\",\"Data\":" + file + "}");
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        online.isConnected = true;
        firstwebload();
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        online.isConnected = false;
        mbParam.loadTable = false;
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
    server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request)
              {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/html", filesystem.readfile("/index.html"));
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
    request->send(response); });
}

void TaskOnlineManager(void *pvParameter)
{
    static long startCheckWifiTime = millis();
    static bool isMessageReceived = false;
    online.qWifiSetting = xQueueCreate(1, sizeof(bool));
    for (;;)
    {
        if (xQueueReceive(online.qWifiSetting, (void *)&isMessageReceived, 1 / portTICK_PERIOD_MS) == pdTRUE)
        {
            WiFi.begin(online.wifi_setting.ssid, online.wifi_setting.pass);
            isMessageReceived = false;
        }
        ws.cleanupClients();
    }
}