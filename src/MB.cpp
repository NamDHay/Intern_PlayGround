#include "Arduino.h"
#include <OnlineManage.h>
#include <MB.h>
#include <ArduinoJson.h>
#include <File_System.h>

#define RTU_MAX_RW 30
#define TCP_MAX_RW 99

#define PLC_MAX_CHAR 40

ModbusRTU mbRTU;
ModbusIP mbTCP;

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
void load_WebTable()
{
    String dataRead = "";
    JsonDocument doc;
    dataRead = filesystem.readfile("/mbSlaveAddressMap.json");

    deserializeJson(doc, dataRead);
    size_t lenght;
    long count = 0;
    for (byte i = 0; i < mbParam.numSlave; i++)
    {
        lenght = mbParam.slave[i].ReadAddress.endAddress - mbParam.slave[i].ReadAddress.startAddress + 1;
        count = mbParam.slave[i].ReadAddress.startAddress;
        for (byte j = 0; j < lenght; j++)
        {
            if (count > mbParam.slave[i].ReadAddress.endAddress)
            {
                Serial.println("Out of range");
                break;
            }
            mbParam.slave[i].typeData[j] = doc[i]["Data"][j][1];
            switch (mbParam.slave[i].typeData[j])
            {
            case COIL_TYPE:
                count++;
                break;
            case WORD_TYPE:
                count += 1;
                break;
            case DWORDS_TYPE:
                count += 2;
                break;
            case FLOAT_TYPE:
                count += 2;
                break;
            case CHAR_TYPE:
                count += 20;
                break;
            default:
                break;
            }
        }
    }
}
void MODBUS_PARAMETER::update_WebTable()
{
    JsonDocument Doc;
    String fbDataString = "";
    size_t lenght;
    long count = 0;
    if (online.isConnected == true)
    {
        for (byte i = 0; i < mbParam.numSlave; i++)
        {
            lenght = mbParam.slave[i].ReadAddress.endAddress - mbParam.slave[i].ReadAddress.startAddress + 1;
            count = mbParam.slave[i].ReadAddress.startAddress;
            Doc["Data"][i]["ID"] = mbParam.slave[i].ID;
            for (byte j = 0; j < lenght; j++)
            {
                if (count > mbParam.slave[i].ReadAddress.endAddress)
                {
                    Serial.println("Out of range");
                    break;
                }
                Doc["Data"][i]["Data"][j][0] = count; // address of register
                Doc["Data"][i]["Data"][j][1] = mbParam.slave[i].typeData[j];
                switch (mbParam.slave[i].typeData[j])
                {
                case COIL_TYPE:
                    count++;
                    break;
                case WORD_TYPE:
                    count += 1;
                    break;
                case DWORDS_TYPE:
                    count += 2;
                    break;
                case FLOAT_TYPE:
                    count += 2;
                    break;
                case CHAR_TYPE:
                    count += 20;
                    break;
                default:
                    break;
                }
            }
        }
        serializeJson(Doc, fbDataString);
        filesystem.writefile("/mbSlaveAddressMap.json", Doc["Data"].as<String>(), 0);
    }
}
void update_ModbusData_Interval()
{
    char readChar[40];
    union f2w_t
    {
        float f;
        uint16_t w[2];
    } f2w;
    union dw2w_t
    {
        int32_t dw;
        uint16_t w[2];
    } dw2w;
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
            if (!mbRTU.slave())
            {
                while (modbusRTU.read_Multiple_Data(mbParam.slave[i].ID.toInt(),
                                                    (uint16_t *)&mbParam.slave[i].Data,
                                                    mbParam.slave[i].ReadAddress.startAddress,
                                                    lenght) != true)
                    ;
                if (modbusRTU.isConnected == true)
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
                mbRTU.task();
                continue;
            }
            mbRTU.task();
        }
        else if ((modbusTCP.client == 1) && (mbParam.slave[i].ID.length() > 5))
        {
            if (mbTCP.isConnected(modbusTCP.str2IP(mbParam.slave[i].ID)))
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
                mbTCP.task();
                mbTCP.connect(modbusTCP.str2IP(mbParam.slave[i].ID));
                Doc["Data"][i]["connect"] = 0;
                continue;
            }
            mbTCP.task();
        }
        if (online.isConnected == true)
        {
            for (byte j = 0; j < lenght; j++)
            {
                if (count > (lenght - 1))
                {
                    Serial.println("Out of range");
                    break;
                }
                switch (mbParam.slave[i].typeData[j])
                {
                case COIL_TYPE:
                    Doc["Data"][i]["Data"][j] = CHECKCOIL(mbParam.slave[i].Data[count], 0);
                    count++;
                    break;
                case WORD_TYPE:
                    Doc["Data"][i]["Data"][j] = mbParam.slave[i].Data[count];
                    count++;
                    break;
                case DWORDS_TYPE:
                    dw2w.w[0] = mbParam.slave[i].Data[count];
                    dw2w.w[1] = mbParam.slave[i].Data[count + 1];
                    Doc["Data"][i]["Data"][j] = dw2w.dw;
                    count += 2;
                    break;
                case FLOAT_TYPE:
                    f2w.w[0] = mbParam.slave[i].Data[count];
                    f2w.w[1] = mbParam.slave[i].Data[count + 1];
                    Doc["Data"][i]["Data"][j] = f2w.f;
                    count += 2;
                    break;
                case CHAR_TYPE:
                    memset(readChar, 0, 40);
                    mbParam.u16_to_c(readChar, &mbParam.slave[i].Data[count]);
                    Serial.println(String(readChar));
                    Doc["Data"][i]["Data"][j] = String(readChar);
                    count += 20;
                    break;
                default:
                    break;
                }
            }
            count = 0;
        }
    }
    serializeJson(Doc, fbDataString);
    // Serial.println(fbDataString);
    if (mbParam.loadTable == true)
        online.notifyClients(fbDataString);
}
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
        writeDoc["Slave"][i]["ws"] = mbParam.slave[i].WriteAddress.startAddress;
        writeDoc["Slave"][i]["we"] = mbParam.slave[i].WriteAddress.endAddress;
    }

    serializeJson(writeDoc, setting);
    Serial.println("JSON: " + setting);
    filesystem.writefile("/mbSlave.json", setting, 0);
}
void MODBUS_PARAMETER::loadSlave()
{
    String dataRead = "";
    JsonDocument doc;
    dataRead = filesystem.readfile("/mbSlave.json");
    // Serial.println(dataRead);
    deserializeJson(doc, dataRead);

    mbParam.numSlave = doc["numSlave"];

    for (byte i = 0; i < mbParam.numSlave; i++)
    {
        mbParam.slave[i].ID = doc["Slave"][i]["id"].as<String>();
        mbParam.slave[i].ReadAddress.startAddress = doc["Slave"][i]["rs"].as<long>();
        mbParam.slave[i].ReadAddress.endAddress = doc["Slave"][i]["re"].as<long>();
        mbParam.slave[i].WriteAddress.startAddress = doc["Slave"][i]["ws"].as<long>();
        mbParam.slave[i].WriteAddress.endAddress = doc["Slave"][i]["we"].as<long>();
    }

    mbParam.writeSlave();
}
void MODBUS_PARAMETER::c_to_u16(char *c_arr, uint16_t *u_arr)
{
    for (int i = 0; i < 20; i++)
    {
        u_arr[i] |= c_arr[2 * i + 1];
        u_arr[i] = u_arr[i] << 8;
        u_arr[i] |= c_arr[2 * i];
        Serial.print(String(u_arr[i]) + "|");
    }
    Serial.println();
}
void MODBUS_PARAMETER::u16_to_c(char *c_arr, uint16_t *u_arr)
{
    for (int i = 0; i < 20; i++)
    {
        c_arr[2 * i + 1] |= u_arr[i] >> 8;
        c_arr[2 * i] |= u_arr[i];
        Serial.print(String(c_arr[2 * i + 1]) + "|" + String(c_arr[2 * i]) + "|");
    }
    Serial.println();
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
        modbusRTU.isConnected = false;
    }
    else
    {
        // Serial.print("Connection");
        modbusRTU.isConnected = true;
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
void MODBUS_RTU::SlaveInit(HardwareSerial *port, unsigned long baud)
{
    modbusRTU.config.port = port;
    modbusRTU.config.baud = baud;
    port->begin(baud, SERIAL_8N1);
    mbRTU.begin(port);
}
void MODBUS_RTU::loadSetting()
{
    String dataRead = "";
    JsonDocument doc;
    dataRead = filesystem.readfile("/mbrtusetting.json");
    // Serial.println(dataRead);
    deserializeJson(doc, dataRead);

    String modbustype = doc["modbustype"].as<String>();

    String baud = doc["baud"].as<String>();
    String serial = doc["serial"].as<String>();
    String mbmaster = doc["mbmaster"].as<String>();

    modbusRTU.master = (mbmaster == "0") ? 0 : 1;
    modbusRTU.config.baud = baud.toInt();
    modbusRTU.config.port = (serial == "0") ? &Serial1 : &Serial2;

    // modbusRTU.writeSetting();
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
    Serial.println("JSON: " + setting);
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
/**********************************************END RTU PART***************************************************************/

/*********************************************START TCP PART**************************************************************/
#define ETH_ADDR 1
#define ETH_POWER_PIN -1 // Do not use it, it can cause conflict during the software reset.
#define ETH_POWER_PIN_ALTERNATIVE 14
#define ETH_MDC_PIN 23
#define ETH_MDIO_PIN 18
#define ETH_TYPE ETH_PHY_LAN8720
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN
#define BUZZ 12
#define BUZZ_ON digitalWrite(BUZZ, HIGH)
#define BUZZ_OFF digitalWrite(BUZZ, LOW)

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
void MODBUS_TCP::ServerInit()
{
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
    // Serial.println(dataRead);
    deserializeJson(doc, dataRead);

    String modbustype = doc["modbustype"].as<String>();

    String ethip = doc["ethip"].as<String>();
    String gw = doc["gw"].as<String>();
    String sn = doc["sn"].as<String>();
    String dns = doc["dns"].as<String>();
    String mbclient = doc["mbclient"].as<String>();

    modbusTCP.client = (mbclient == "0") ? 0 : 1;
    modbusTCP.ethernet.ipAdress = ethip;
    modbusTCP.ethernet.gateway = gw;
    modbusTCP.ethernet.subnet = sn;
    modbusTCP.ethernet.primaryDNS = dns;

    // modbusTCP.writeSetting();
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
    Serial.println("JSON: " + setting);
    filesystem.writefile("/mbtcpsetting.json", setting, 0);
}
/*********************************************END TCP PART****************************************************************/

/*********************************************START MODBUS TASK***********************************************************/
void TaskModbus(void *pvParameter)
{
    String dataRead = "";
    JsonDocument doc;
    mbParam.loadTable = false;
    modbusRTU.loadSetting();
    modbusTCP.loadSetting();
    mbParam.loadSlave();
    load_WebTable();

    modbusRTU.qUpdateTable = xQueueCreate(1, sizeof(bool));
    modbusTCP.qUpdateTable = xQueueCreate(1, sizeof(bool));

    modbusTCP.EthernetInit();

    modbusRTU.MasterInit(modbusRTU.config.port, modbusRTU.config.baud);
    modbusTCP.ClientInit();

    static long startUpdateRTUIntervalTime = millis();
    static long startUpdateTCPIntervalTime = millis();
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