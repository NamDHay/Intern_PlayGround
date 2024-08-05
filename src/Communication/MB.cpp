#include "Arduino.h"
#include <OnlineManage/OnlineManage.h>
#include <Communication/MB.h>
#include <ArduinoJson.h>
#include <FileSystem/FileSystem.h>
#define UPDATE_INTERVAL_MS 500
#define RTU_MAX_RW 30
#define TCP_MAX_RW 99

ModbusRTU mbRTU;
ModbusIP mbTCP;

/*********************************************START MBPARAM PART**************************************************************/
void MODBUS_PARAMETER::writeSlave()
{
    String setting = "";
    JsonDocument writeDoc;

    writeDoc["numSlave"] = mbParam.numSlave;
    for (byte i = 0; i < mbParam.numSlave; i++)
    {
        writeDoc["Slave"][i]["id"] = mbParam.slave[i].ID;
        writeDoc["Slave"][i]["rs"] = mbParam.slave[i].ReadAddress.startAddress;
        writeDoc["Slave"][i]["re"] = mbParam.slave[i].ReadAddress.endAddress;
    }

    serializeJson(writeDoc, setting);
    filesystem.writefile("/mbSlave.json", setting, 0);
}
void MODBUS_PARAMETER::loadSlave()
{
    String dataRead = "";
    JsonDocument doc;
    dataRead = filesystem.readfile("/mbSlave.json");
    deserializeJson(doc, dataRead);

    mbParam.numSlave = doc["numSlave"];

    for (byte i = 0; i < mbParam.numSlave; i++)
    {
        mbParam.slave[i].ID = doc["Slave"][i]["id"].as<String>();
        mbParam.slave[i].ReadAddress.startAddress = doc["Slave"][i]["rs"].as<long>();
        mbParam.slave[i].ReadAddress.endAddress = doc["Slave"][i]["re"].as<long>();
    }
}
/**********************************************END MBPARAM PART**************************************************************/

/*********************************************START RTU PART**************************************************************/
bool cb(Modbus::ResultCode event, uint16_t transactionId, void *data) // Callback to monitor errors
{
    if (event != Modbus::EX_SUCCESS)
    {
        // Serial.print("Request result: 0x");
        // Serial.println(event, HEX);
        // Serial.print("No Connection");
        modbusRTU.isConnect = false;
    }
    else
    {
        modbusRTU.isConnect = true;
    }
    return true;
}
void MODBUS_RTU::MasterInit(HardwareSerial *port, unsigned long baud)
{
    modbusRTU.config.port = port;
    modbusRTU.config.baud = baud;
    port->begin(baud, SERIAL_8N1);
    mbRTU.begin(port);
    mbRTU.master();
}
void MODBUS_RTU::loadSetting()
{
    String dataRead = "";
    JsonDocument doc;
    dataRead = filesystem.readfile("/mbrtusetting.json");
    deserializeJson(doc, dataRead);

    String modbustype = doc["modbustype"].as<String>();

    String baud = doc["baud"].as<String>();
    String serial = doc["serial"].as<String>();
    String mbmaster = doc["mbmaster"].as<String>();

    modbusRTU.master = (mbmaster == "1") ? 1 : 0;
    modbusRTU.config.baud = baud.toInt();
    modbusRTU.config.port = (serial == "0") ? &Serial1 : &Serial2;
    if (modbusRTU.master == 1)
    {
        modbusRTU.MasterInit(modbusRTU.config.port, modbusRTU.config.baud);
    }
    else
    {
        modbusRTU.MasterInit();
    }
}
void MODBUS_RTU::writeSetting()
{
    String setting = "";
    JsonDocument writeDoc;

    writeDoc["modbustype"] = ((modbusTCP.client == 1) && (modbusRTU.master == 1)) ? "2" : (modbusTCP.client == 1) ? "1"
                                                                                                                  : "0";
    writeDoc["baud"] = modbusRTU.config.baud;
    writeDoc["serial"] = (modbusRTU.config.port == &Serial1) ? "0" : "1";
    writeDoc["mbmaster"] = modbusRTU.master;

    serializeJson(writeDoc, setting);
    filesystem.writefile("/mbrtusetting.json", setting, 0);
}
bool MODBUS_RTU::read_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length)
{
    static uint16_t rstart;
    static long dataLength = 0;
    static uint16_t *rdata = NULL;
    if (!length || !value)
        return false;
    if (!dataLength || !rdata)
    {
        rstart = startAddress;
        dataLength = length;
        rdata = value;
    }
    if (dataLength >= RTU_MAX_RW)
    {
        mbRTU.readHreg(ID,
                       rstart,
                       rdata,
                       RTU_MAX_RW,
                       cb);
        while (mbRTU.slave())
        {
            mbRTU.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        rstart = rstart + RTU_MAX_RW;
        dataLength -= RTU_MAX_RW;
        rdata += RTU_MAX_RW;
        if (dataLength == 0)
        {
            return true;
        }
        return false;
    }
    else if (dataLength < RTU_MAX_RW)
    {
        mbRTU.readHreg(ID,
                       rstart,
                       rdata,
                       dataLength,
                       cb);
        while (mbRTU.slave())
        {
            mbRTU.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        dataLength = 0;
        rstart = 0;
        rdata = NULL;
        return true;
    }
    return false;
}
bool MODBUS_RTU::write_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length)
{
    static uint16_t wstart;
    static long wdataLength = 0;
    static uint16_t *wdata = NULL;
    if (!length || !value)
        return false;
    if (!wdataLength || !wdata)
    {
        wstart = startAddress;
        wdataLength = length;
        wdata = value;
    }
    if (wdataLength >= RTU_MAX_RW)
    {
        mbRTU.writeHreg(ID,
                        wstart,
                        wdata,
                        RTU_MAX_RW,
                        cb);
        while (mbRTU.slave())
        {
            mbRTU.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        wstart = wstart + RTU_MAX_RW;
        wdataLength -= RTU_MAX_RW;
        wdata += RTU_MAX_RW;
        if (wdataLength == 0)
        {
            return true;
        }
        return false;
    }
    else if (wdataLength < RTU_MAX_RW)
    {
        mbRTU.writeHreg(ID,
                        wstart,
                        wdata,
                        wdataLength,
                        cb);
        while (mbRTU.slave())
        {
            mbRTU.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        wdataLength = 0;
        wstart = 0;
        wdata = NULL;
        return true;
    }
    return false;
}
void MODBUS_RTU::task()
{
    mbRTU.task();
}
uint8_t MODBUS_RTU::slave()
{
    return mbRTU.slave();
}
/**********************************************END RTU PART***************************************************************/

/*********************************************START TCP PART**************************************************************/
#define ETH_ADDR 1
#define ETH_POWER_PIN -1 // Do not use it, it can cause conflict during the software reset.
#define ETH_POWER_PIN_ALTERNATIVE 14
#define ETH_MDC_PIN 23
#define ETH_MDIO_PIN 18
#define ETH_TYPE ETH_PHY_LAN8720
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN

static bool eth_connected = false;
void WiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
    case SYSTEM_EVENT_ETH_START:
        Serial.println("ETH Started");
        // set eth hostname here
        ETH.setHostname("esp32-ethernet");
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        Serial.println("ETH Connected");
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        Serial.print("ETH MAC: ");
        Serial.print(ETH.macAddress());
        Serial.print(", IPv4: ");
        Serial.print(ETH.localIP());
        if (ETH.fullDuplex())
        {
            Serial.print(", FULL_DUPLEX");
        }
        Serial.print(", ");
        Serial.print(ETH.linkSpeed());
        Serial.println("Mbps");
        eth_connected = true;
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        Serial.println("ETH Disconnected");
        eth_connected = false;
        break;
    case SYSTEM_EVENT_ETH_STOP:
        Serial.println("ETH Stopped");
        eth_connected = false;
        break;
    default:
        break;
    }
}
void MODBUS_TCP::EthernetInit()
{
    Serial.println("Connect Lan...");
    pinMode(ETH_POWER_PIN_ALTERNATIVE, OUTPUT);
    digitalWrite(ETH_POWER_PIN_ALTERNATIVE, HIGH);
    WiFi.onEvent(WiFiEvent);
    ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
    ETH.config(str2IP(modbusTCP.ethernet.ipAdress),
               str2IP(modbusTCP.ethernet.gateway),
               str2IP(modbusTCP.ethernet.subnet),
               str2IP(modbusTCP.ethernet.primaryDNS));
}
void MODBUS_TCP::ClientInit()
{
    mbTCP.client();
}
bool MODBUS_TCP::read_Multiple_Data(IPAddress ID, uint16_t *value, long startAddress, size_t length)
{
    static uint16_t rstart;
    static long dataLength = 0;
    static uint16_t *rdata = NULL;
    if (!length || !value)
        return false;
    if (!dataLength || !rdata)
    {
        rstart = startAddress;
        dataLength = length;
        rdata = value;
    }
    if (dataLength >= TCP_MAX_RW)
    {
        mbTCP.readHreg(ID,
                       rstart,
                       rdata,
                       TCP_MAX_RW);
        mbTCP.task();
        rstart = rstart + TCP_MAX_RW;
        dataLength -= TCP_MAX_RW;
        rdata += TCP_MAX_RW;
        if (dataLength == 0)
        {
            return true;
        }
        return false;
    }
    else if (dataLength < TCP_MAX_RW)
    {
        mbTCP.readHreg(ID,
                       rstart,
                       rdata,
                       dataLength);
        mbTCP.task();
        dataLength = 0;
        rstart = 0;
        rdata = NULL;
        return true;
    }
    return false;
}
bool MODBUS_TCP::write_Multiple_Data(IPAddress ID, uint16_t *value, long startAddress, size_t length)
{
    static uint16_t wstart;
    static long wdataLength = 0;
    static uint16_t *wdata = NULL;
    if (!length || !value)
        return false;
    if (!wdataLength || !wdata)
    {
        wstart = startAddress;
        wdataLength = length;
        wdata = value;
    }
    if (wdataLength >= TCP_MAX_RW)
    {
        mbTCP.writeHreg(ID,
                        wstart,
                        wdata,
                        TCP_MAX_RW);
        mbTCP.task();

        wstart = wstart + TCP_MAX_RW;
        wdataLength -= TCP_MAX_RW;
        wdata += TCP_MAX_RW;
        if (wdataLength == 0)
        {
            return true;
        }
        return false;
    }
    else if (wdataLength < TCP_MAX_RW)
    {
        mbTCP.writeHreg(ID,
                        wstart,
                        wdata,
                        wdataLength);
        mbTCP.task();
        wdataLength = 0;
        wstart = 0;
        wdata = NULL;
        return true;
    }
    return false;
}
void MODBUS_TCP::loadSetting()
{
    String dataRead = "";
    JsonDocument doc;
    dataRead = filesystem.readfile("/mbtcpsetting.json");
    deserializeJson(doc, dataRead);

    String modbustype = doc["modbustype"].as<String>();

    String ethip = doc["ethip"].as<String>();
    String gw = doc["gw"].as<String>();
    String sn = doc["sn"].as<String>();
    String dns = doc["dns"].as<String>();
    String mbclient = doc["mbclient"].as<String>();

    modbusTCP.client = (mbclient == "1") ? 1 : 0;
    modbusTCP.ethernet.ipAdress = ethip;
    modbusTCP.ethernet.gateway = gw;
    modbusTCP.ethernet.subnet = sn;
    modbusTCP.ethernet.primaryDNS = dns;

    if (modbusTCP.client == 1)
    {
        modbusTCP.EthernetInit();
        modbusTCP.ClientInit();
    }
}
void MODBUS_TCP::writeSetting()
{
    String setting = "";
    JsonDocument writeDoc;

    writeDoc["modbustype"] = ((modbusTCP.client == 1) && (modbusRTU.master == 1)) ? "2" : (modbusTCP.client == 1) ? "1"
                                                                                                                  : "0";

    writeDoc["mbclient"] = modbusTCP.client;
    writeDoc["ethip"] = modbusTCP.ethernet.ipAdress;
    writeDoc["gw"] = modbusTCP.ethernet.gateway;
    writeDoc["sn"] = modbusTCP.ethernet.subnet;
    writeDoc["dns"] = modbusTCP.ethernet.primaryDNS;

    serializeJson(writeDoc, setting);
    filesystem.writefile("/mbtcpsetting.json", setting, 0);
}
void MODBUS_TCP::task()
{
    mbTCP.task();
}
bool MODBUS_TCP::connect(IPAddress address)
{
    return mbTCP.connect(address);
}
bool MODBUS_TCP::isConnected(IPAddress address)
{
    return mbTCP.isConnected(address);
}
/*********************************************END TCP PART****************************************************************/

/*********************************************START MODBUS TASK***********************************************************/
int getIpBlock(int index, String str)
{
    char separator = '.';
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = str.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (str.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? str.substring(strIndex[0], strIndex[1]).toInt() : 0;
}
IPAddress MODBUS_TCP::str2IP(String str)
{
    IPAddress ret(getIpBlock(0, str), getIpBlock(1, str), getIpBlock(2, str), getIpBlock(3, str));
    return ret;
}
void update_ModbusData_Interval()
{
    JsonDocument Doc;
    String fbDataString = "";
    long count = 0;
    size_t lenght;
    Doc["Command"] = "tableData";
    for (byte i = 0; i < mbParam.numSlave; i++)
    {
        lenght = mbParam.slave[i].ReadAddress.endAddress - mbParam.slave[i].ReadAddress.startAddress + 1;
        Doc["Data"][i]["ID"] = mbParam.slave[i].ID;
        if ((modbusRTU.master == 1) && (mbParam.slave[i].ID.length() < 5))
        {
            if (!modbusRTU.slave())
            {
                while (modbusRTU.read_Multiple_Data(mbParam.slave[i].ID.toInt(),
                                                    (uint16_t *)&mbParam.slave[i].Data,
                                                    mbParam.slave[i].ReadAddress.startAddress,
                                                    lenght) != true)
                    ;
                if (modbusRTU.isConnect == true)
                {
                    Doc["Data"][i]["connect"] = 1;
                }
                else
                {
                    Doc["Data"][i]["connect"] = 0;
                    continue;
                }
            }
            else
            {
                modbusRTU.task();
                continue;
            }
            modbusRTU.task();
        }
        else if ((modbusTCP.client == 1) && (mbParam.slave[i].ID.length() > 5))
        {
            if (modbusTCP.isConnected(modbusTCP.str2IP(mbParam.slave[i].ID)))
            {
                Doc["Data"][i]["connect"] = 1;
                while (modbusTCP.read_Multiple_Data(modbusTCP.str2IP(mbParam.slave[i].ID),
                                                    (uint16_t *)&mbParam.slave[i].Data,
                                                    mbParam.slave[i].ReadAddress.startAddress,
                                                    lenght) != true)
                    ;
            }
            else
            {
                modbusTCP.task();
                modbusTCP.connect(modbusTCP.str2IP(mbParam.slave[i].ID));
                Doc["Data"][i]["connect"] = 0;
                continue;
            }
            modbusTCP.task();
        }
        if (online.isConnected == true)
        {
            for (byte j = 0; j < lenght; j++)
            {
                Doc["Data"][i]["Data"][j] = mbParam.slave[i].Data[j];
            }
            count = 0;
        }
    }
    if (mbParam.loadTable == true)
    {
        serializeJson(Doc, fbDataString);
        online.notifyClients(fbDataString);
    }
}
void TaskModbus(void *pvParameter)
{
    String dataRead = "";
    JsonDocument doc;
    mbParam.loadTable = false;
    modbusRTU.loadSetting();
    modbusTCP.loadSetting();
    mbParam.loadSlave();

    static long startUpdateIntervalTime = millis();
    for (;;)
    {
        if ((millis() - startUpdateIntervalTime >= UPDATE_INTERVAL_MS))
        {
            update_ModbusData_Interval();
            startUpdateIntervalTime = millis();
        }
    }
}
/*********************************************END MODBUS TASK*************************************************************/