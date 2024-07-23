#include "Arduino.h"
#include <OnlineManage.h>
#include <MB.h>
#include <ArduinoJson.h>
#include <File_System.h>

#define WORD_TYPE 0
#define COIL_TYPE 1
#define DWORDS_TYPE 2
#define FLOAT_TYPE 3

#define RTU_MAX_RW 30
#define TCP_MAX_RW 99

#define CHECKCOIL(CoilGroup, CoilBit) ((((CoilGroup) & (1UL << CoilBit)) == (1UL << CoilBit)) ? 1 : 0)
#define SETCOIL(CoilGroup, CoilBit) ((CoilGroup) |= (1UL << CoilBit))
#define CLEARCOIL(CoilGroup, CoilBit) (CoilGroup &= ~(1UL << CoilBit))

ModbusRTU mbRTU;
ModbusIP mbTCP;

uint16_t Master_ReadData[200];
uint16_t Master_WriteData[200];
uint16_t Slave_ReadData[200];
uint16_t Slave_WriteData[200];
uint16_t Client_ReadData[200];
uint16_t Client_WriteData[200];
uint16_t Server_ReadData[200];
uint16_t Server_WriteData[200];
uint16_t TxData_ReadData[200];

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
IPAddress str2IP(String str)
{
    IPAddress ret(getIpBlock(0, str), getIpBlock(1, str), getIpBlock(2, str), getIpBlock(3, str));
    return ret;
}
void update_WebTable(long startAddress, long endAddress, uint8_t *type, String ID)
{
    JsonDocument Doc;
    String fbDataString = "";
    size_t length = (endAddress - startAddress + 1);
    Serial.println("Data Length: " + String(length));

    if (online.isConnected == true)
    {
        Doc["Command"] = "TableID";
        Doc["ID"] = ID;
        for (uint8_t i = 0; i < length; i++)
        {
            if (startAddress > endAddress)
            {
                Serial.println("Out of range");
                break;
            }
            Doc["Data"][i]["address"] = startAddress;
            Doc["Data"][i]["type"] = type[i];
            switch (type[i])
            {
            case COIL_TYPE:
                startAddress++;
                break;
            case WORD_TYPE:
                startAddress += 1;
                break;
            case DWORDS_TYPE:
                startAddress += 2;
                break;
            case FLOAT_TYPE:
                startAddress += 2;
                break;
            default:
                break;
            }
        }
        serializeJson(Doc, fbDataString);
        serializeJsonPretty(Doc, Serial);
        online.notifyClients(fbDataString);
    }
}
// void update_WebData_Interval(long startAddress, long endAddress, uint16_t *data, uint8_t *type, String ID) // Must be void function to avoid control reaches end of non-void function [-Wreturn-type]
// {
//     // Serial.print("startAddress" + String(startAddress));
//     // Serial.print("|");
//     // Serial.print("endAddress" + String(endAddress));
//     // Serial.print("|");
//     // for (long i = 0; i < (endAddress - startAddress + 1); i++)
//     // {
//     //     Serial.print(data[i]);
//     //     Serial.print("|");
//     // }
//     // Serial.println();
//     union f2w_t
//     {
//         float f;
//         uint16_t w[2];
//     } f2w;
//     union dw2w_t
//     {
//         int32_t dw;
//         uint16_t w[2];
//     } dw2w;
//     JsonDocument Doc; // this guy make error messages if not return void
//     String fbDataString = "";
//     size_t length = (endAddress - startAddress + 1);
//     long count = 0;
//     if (online.isConnected == true)
//     {
//         Doc["Command"] = "tableData";
//         Doc["ID"] = ID;
//         for (uint8_t i = 0; i < length; i++)
//         {
//             if (startAddress > endAddress)
//             {
//                 Serial.println("Out of range");
//                 break;
//             }

//             switch (type[i])
//             {
//             case COIL_TYPE:
//                 Doc["Data"][i] = CHECKCOIL(data[count], 0);
//                 count++;
//                 startAddress++;
//                 break;
//             case WORD_TYPE:
//                 Doc["Data"][i] = data[count];
//                 startAddress += 1;
//                 count++;
//                 break;
//             case DWORDS_TYPE:
//                 dw2w.w[0] = data[count];
//                 dw2w.w[1] = data[count + 1];
//                 Doc["Data"][i] = dw2w.dw;
//                 startAddress += 2;
//                 count += 2;
//                 break;
//             case FLOAT_TYPE:
//                 f2w.w[0] = data[count];
//                 f2w.w[1] = data[count + 1];
//                 Doc["Data"][i] = f2w.f;
//                 startAddress += 2;
//                 count += 2;
//                 break;
//             default:
//                 break;
//             }
//         }
//         serializeJson(Doc, fbDataString);
//         serializeJsonPretty(Doc, Serial);
//         online.notifyClients(fbDataString);
//     }
// }
void update_ModbusData_Interval()
{
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
    if (online.isConnected == true)
    {
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
                                                        (uint16_t *)&TxData_ReadData,
                                                        modbusRTU.slave[i].ReadAddress.startAddress,
                                                        lenght) != true)
                        ;
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
                if (mbTCP.isConnected(str2IP(mbParam.slave[i].ID)))
                {
                    modbusTCP.isConnected = true;
                    while (modbusRTU.read_Multiple_Data(str2IP(mbParam.slave[i].ID),
                                                        (uint16_t *)&TxData_ReadData,
                                                        modbusRTU.slave[i].ReadAddress.startAddress,
                                                        lenght) != true)
                        ;
                }
                else
                {
                    mbTCP.task();
                    mbTCP.connect(str2IP(mbParam.slave[i].ID));
                    continue;
                }
                mbTCP.task();
            }

            for (byte j = 0; j < lenght; j++)
            {
                if (count > mbParam.slave[i].ReadAddress.endAddress)
                {
                    Serial.println("Out of range");
                    break;
                }
                switch (mbParam.slave->typeData[i])
                {
                case COIL_TYPE:
                    Doc["Data"][i]["Data"][j] = CHECKCOIL(TxData_ReadData[count], 0);
                    count++;
                    break;
                case WORD_TYPE:
                    Doc["Data"][i]["Data"][j] = TxData_ReadData[count];
                    count++;
                    break;
                case DWORDS_TYPE:
                    dw2w.w[0] = TxData_ReadData[count];
                    dw2w.w[1] = TxData_ReadData[count + 1];
                    Doc["Data"][i]["Data"][j] = dw2w.dw;
                    count += 2;
                    break;
                case FLOAT_TYPE:
                    f2w.w[0] = TxData_ReadData[count];
                    f2w.w[1] = TxData_ReadData[count + 1];
                    Doc["Data"][i]["Data"][j] = f2w.f;
                    count += 2;
                    break;
                default:
                    break;
                }
            }
        }
        serializeJson(Doc, fbDataString);
        Serial.println(fbDataString);
        // online.notifyClients(fbDataString);
    }
}
/*********************************************START RTU PART**************************************************************/
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
    deserializeJson(doc, dataRead);

    String modbustype = doc["modbustype"].as<String>();

    String baud = doc["baud"].as<String>();
    String serial = doc["serial"].as<String>();
    String mbmaster = doc["mbmaster"].as<String>();

    modbusRTU.master = (mbmaster == "0") ? 0 : 1;
    modbusRTU.config.baud = baud.toInt();
    modbusRTU.config.port = (serial == "0") ? &Serial1 : &Serial2;

    modbusRTU.writeSetting();
}
// long count = 0;
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
                       RTU_MAX_RW);
        while (mbRTU.slave())
        {
            mbRTU.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        // for (long i = rstart; i < rstart + RTU_MAX_RW; i++)
        // {
        //     Serial.print("  Data " + String(i) + ": ");
        //     Serial.println(rdata[count]);
        //     count++;
        // }
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
                       dataLength);
        while (mbRTU.slave())
        {
            mbRTU.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        // for (long i = rstart; i < rstart + dataLength; i++)
        // {
        //     Serial.print("   Data " + String(i) + ": ");
        //     Serial.println(rdata[count]);
        //     count++;
        // }
        dataLength = 0;
        rstart = 0;
        rdata = NULL;
        // Serial.println("Count = " + String(count));
        // count = 0;
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
                        RTU_MAX_RW);
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
                        wdataLength);
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
void MODBUS_RTU::loadSlave()
{
    String dataRead = "";
    JsonDocument doc;
    dataRead = filesystem.readfile("/rtuslave.json");
    deserializeJson(doc, dataRead);

    modbusRTU.numSlave = doc["numSlave"];

    for (byte i = 0; i < modbusRTU.numSlave; i++)
    {
        modbusRTU.slave[i].ID = doc["Slave"][i]["id"].as<byte>();
        modbusRTU.slave[i].ReadAddress.startAddress = doc["Slave"][i]["rs"].as<long>();
        modbusRTU.slave[i].ReadAddress.endAddress = doc["Slave"][i]["re"].as<long>();
        modbusRTU.slave[i].WriteAddress.startAddress = doc["Slave"][i]["ws"].as<long>();
        modbusRTU.slave[i].WriteAddress.endAddress = doc["Slave"][i]["we"].as<long>();
    }

    modbusRTU.writeSlave();
}
void MODBUS_RTU::writeSlave()
{
    String setting = "";
    JsonDocument writeDoc;

    writeDoc["numSlave"] = modbusRTU.numSlave;
    for (byte i = 0; i < modbusRTU.numSlave; i++)
    {
        writeDoc["Slave"][i]["id"] = modbusRTU.slave[i].ID;
        writeDoc["Slave"][i]["rs"] = modbusRTU.slave[i].ReadAddress.startAddress;
        writeDoc["Slave"][i]["re"] = modbusRTU.slave[i].ReadAddress.endAddress;
        writeDoc["Slave"][i]["ws"] = modbusRTU.slave[i].WriteAddress.startAddress;
        writeDoc["Slave"][i]["we"] = modbusRTU.slave[i].WriteAddress.endAddress;
    }

    serializeJson(writeDoc, setting);
    Serial.println("JSON: " + setting);
    filesystem.writefile("/rtuslave.json", setting, 0);
}
/**************************************************END RTU PART***********************************************************/

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
        // for (uint8_t i = rstart; i < rstart + TCP_MAX_RW; i++)
        // {
        //     Serial.print("Data " + String(i) + ": ");
        //     Serial.println(rdata[i]);
        // }
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
        // for (uint8_t i = rstart; i < rstart + dataLength; i++)
        // {
        //     Serial.print("Data " + String(i) + ": ");
        //     Serial.println(rdata[i]);
        // }
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

    modbusTCP.client = (mbclient == "0") ? 0 : 1;
    modbusTCP.ethernet.ipAdress = ethip;
    modbusTCP.ethernet.gateway = gw;
    modbusTCP.ethernet.subnet = sn;
    modbusTCP.ethernet.primaryDNS = dns;

    modbusTCP.writeSetting();
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
void MODBUS_TCP::loadSlave()
{
    String dataRead = "";
    JsonDocument doc;
    dataRead = filesystem.readfile("/tcpslave.json");
    deserializeJson(doc, dataRead);

    modbusTCP.numSlave = doc["numSlave"];

    for (byte i = 0; i < modbusTCP.numSlave; i++)
    {
        modbusTCP.slave[i].IP = doc["Slave"][i]["ip"].as<String>();
        modbusTCP.slave[i].ReadAddress.startAddress = doc["Slave"][i]["rs"].as<long>();
        modbusTCP.slave[i].ReadAddress.endAddress = doc["Slave"][i]["re"].as<long>();
        modbusTCP.slave[i].WriteAddress.startAddress = doc["Slave"][i]["ws"].as<long>();
        modbusTCP.slave[i].WriteAddress.endAddress = doc["Slave"][i]["we"].as<long>();
    }

    modbusTCP.writeSlave();
}
void MODBUS_TCP::writeSlave()
{
    String setting = "";
    JsonDocument writeDoc;

    writeDoc["numSlave"] = modbusTCP.numSlave;
    for (byte i = 0; i < modbusTCP.numSlave; i++)
    {
        writeDoc["Slave"][i]["ip"] = modbusTCP.slave[i].IP;
        writeDoc["Slave"][i]["rs"] = modbusTCP.slave[i].ReadAddress.startAddress;
        writeDoc["Slave"][i]["re"] = modbusTCP.slave[i].ReadAddress.endAddress;
        writeDoc["Slave"][i]["ws"] = modbusTCP.slave[i].WriteAddress.startAddress;
        writeDoc["Slave"][i]["we"] = modbusTCP.slave[i].WriteAddress.endAddress;
    }

    serializeJson(writeDoc, setting);
    Serial.println("JSON: " + setting);
    filesystem.writefile("/tcpslave.json", setting, 0);
}
/*********************************************END TCP PART****************************************************************/

/*********************************************START MODBUS TASK***********************************************************/
void TaskModbus(void *pvParameter)
{
    modbusRTU.loadSetting();
    modbusTCP.loadSetting();
    modbusRTU.loadSlave();
    modbusTCP.loadSlave();

    /*Start Test Part*/
    // modbusTCP.client = 0;
    // modbusRTU.master = 1;

    // modbusRTU.MasterReadReg.startAddress = 0;
    // modbusRTU.MasterReadReg.endAddress = 99;
    // modbusRTU.MasterWriteReg.startAddress = 100;
    // modbusRTU.MasterWriteReg.endAddress = 199;
    // modbusRTU.SlaveReadReg.startAddress = 0;
    // modbusRTU.SlaveReadReg.endAddress = 99;
    // modbusRTU.SlaveWriteReg.startAddress = 100;
    // modbusRTU.SlaveWriteReg.endAddress = 199;

    // modbusTCP.ethernet.ipAdress = "192.168.137.3";
    // modbusTCP.ethernet.gateway = "192.168.137.1";
    // modbusTCP.ethernet.subnet = "255.255.255.0";
    // modbusTCP.ethernet.primaryDNS = "8.8.8.8";
    // modbusTCP.remote = "192.168.137.2";

    // modbusRTU.config.port = &Serial2;
    // modbusRTU.config.baud = 9600;

    // modbusTCP.ClientReadReg.startAddress = 0;
    // modbusTCP.ClientReadReg.endAddress = 99;
    // modbusTCP.ClientWriteReg.startAddress = 100;
    // modbusTCP.ClientWriteReg.endAddress = 199;
    /*End Test Part*/

    bool setTable = false;
    modbusRTU.loadTable = true;
    modbusTCP.loadTable = true;
    byte RTUSlaveCount = 0;
    byte TCPSlaveCount = 0;
    byte SlaveCount = 0;

    size_t reglengh = (modbusRTU.slave[0].WriteAddress.endAddress - modbusRTU.slave[0].WriteAddress.startAddress + 1);
    modbusRTU.qUpdateTable = xQueueCreate(1, sizeof(bool));
    modbusTCP.qUpdateTable = xQueueCreate(1, sizeof(bool));
    mbParam.qUpdateTable = xQueueCreate(1, sizeof(bool));

    modbusTCP.EthernetInit();

    static long startUpdateRTUIntervalTime = millis();
    static long startUpdateTCPIntervalTime = millis();
    static long startUpdateIntervalTime = millis();
    modbusRTU.MasterInit(modbusRTU.config.port, modbusRTU.config.baud);
    modbusTCP.ClientInit();

    for (;;)
    {
        // if (modbusRTU.master == 1) // Master part
        // {
        //     if (!mbRTU.slave())
        //     {
        //         modbusRTU.isConnected = true;
        //         while (modbusRTU.read_Multiple_Data(modbusRTU.slave[RTUSlaveCount].ID.toInt(),
        //                                             (uint16_t *)&Master_ReadData,
        //                                             modbusRTU.slave[RTUSlaveCount].ReadAddress.startAddress,
        //                                             (modbusRTU.slave[RTUSlaveCount].ReadAddress.endAddress - modbusRTU.slave[RTUSlaveCount].ReadAddress.startAddress + 1)) != true)
        //             ;
        //     }
        //     else
        //     {
        //         modbusRTU.isConnected = false;
        //     }
        //     mbRTU.task();
        //     RTUSlaveCount++;
        //     if (RTUSlaveCount >= modbusRTU.numSlave)
        //         RTUSlaveCount = 0;
        //     SlaveCount++;
        //     if (SlaveCount >= mbParam.numSlave)
        //         SlaveCount = 0;
        // } // Master part

        // if (modbusTCP.client == 1) // Client part
        // {
        //     if (mbTCP.isConnected(str2IP(modbusTCP.slave[TCPSlaveCount].IP)))
        //     {
        //         modbusTCP.isConnected = true;
        //         while (modbusTCP.read_Multiple_Data(str2IP(modbusTCP.slave[TCPSlaveCount].IP),
        //                                             (uint16_t *)&Client_ReadData,
        //                                             modbusTCP.slave[TCPSlaveCount].ReadAddress.startAddress,
        //                                             (modbusTCP.slave[TCPSlaveCount].ReadAddress.endAddress - modbusTCP.slave[TCPSlaveCount].ReadAddress.startAddress + 1)) != true)
        //             ;
        //     }
        //     else
        //     {
        //         mbTCP.connect(str2IP(modbusTCP.slave[TCPSlaveCount].IP));
        //         modbusTCP.isConnected = false;
        //     }
        //     mbTCP.task();
        //     TCPSlaveCount++;
        //     if (TCPSlaveCount >= modbusTCP.numSlave)
        //         TCPSlaveCount = 0;
        //     SlaveCount++;
        //     if (SlaveCount >= mbParam.numSlave)
        //         SlaveCount = 0;
        // } // Client part
        // if ((millis() - startUpdateTCPIntervalTime >= UPDATE_INTERVAL_MS) && (modbusTCP.loadTable == true))
        // {
        //     startUpdateTCPIntervalTime = millis();
        //     update_WebData_Interval((modbusTCP.slave[TCPSlaveCount].ReadAddress.startAddress),
        //                             (modbusTCP.slave[TCPSlaveCount].ReadAddress.endAddress),
        //                             ((uint16_t *)&Client_ReadData),
        //                             (modbusTCP.slave[TCPSlaveCount].typeData),
        //                             String(modbusTCP.slave[TCPSlaveCount].IP));
        // }
        // if ((millis() - startUpdateRTUIntervalTime >= UPDATE_INTERVAL_MS) && (modbusRTU.loadTable == true))
        // {
        //     startUpdateRTUIntervalTime = millis();
        //     update_WebData_Interval((modbusRTU.slave[RTUSlaveCount].ReadAddress.startAddress),
        //                             (modbusRTU.slave[RTUSlaveCount].ReadAddress.endAddress),
        //                             ((uint16_t *)&Master_ReadData),
        //                             (modbusRTU.slave[RTUSlaveCount].typeData),
        //                             String(modbusRTU.slave[RTUSlaveCount].ID));
        // }
        // if (xQueueReceive(modbusRTU.qUpdateTable, (void *)&setTable, 1 / portTICK_PERIOD_MS) == pdTRUE)
        // {
        //     for (uint8_t i = 0; i < modbusRTU.numSlave; i++)
        //     {
        //         update_WebTable((modbusRTU.slave[i].ReadAddress.startAddress),
        //                         (modbusRTU.slave[i].ReadAddress.endAddress),
        //                         (modbusRTU.slave[i].typeData),
        //                         String(modbusRTU.slave[i].ID));
        //     }
        //     setTable = false;
        //     modbusRTU.loadTable = true;
        // }
        // if (xQueueReceive(modbusTCP.qUpdateTable, (void *)&setTable, 1 / portTICK_PERIOD_MS) == pdTRUE)
        // {
        //     for (uint8_t i = 0; i < modbusTCP.numSlave; i++)
        //     {
        //         update_WebTable((modbusTCP.slave[i].ReadAddress.startAddress),
        //                         (modbusTCP.slave[i].ReadAddress.endAddress),
        //                         (modbusTCP.slave[i].typeData),
        //                         String(modbusTCP.slave[i].IP));
        //     }
        //     setTable = false;
        //     modbusTCP.loadTable = true;
        // }

        update_ModbusData_Interval();
        if ((millis() - startUpdateIntervalTime >= UPDATE_INTERVAL_MS) && (mbParam.loadTable == true))
        {
            startUpdateIntervalTime = millis();
            // update_WebData_Interval((modbusRTU.slave[RTUSlaveCount].ReadAddress.startAddress),
            //                         (modbusRTU.slave[RTUSlaveCount].ReadAddress.endAddress),
            //                         ((uint16_t *)&Master_ReadData),
            //                         (modbusRTU.slave[RTUSlaveCount].typeData),
            //                         String(modbusRTU.slave[RTUSlaveCount].ID));
        }
        if (xQueueReceive(mbParam.qUpdateTable, (void *)&setTable, 1 / portTICK_PERIOD_MS) == pdTRUE)
        {
            for (uint8_t i = 0; i < mbParam.numSlave; i++)
            {
                update_WebTable((modbusTCP.slave[i].ReadAddress.startAddress),
                                (modbusTCP.slave[i].ReadAddress.endAddress),
                                (modbusTCP.slave[i].typeData),
                                String(modbusTCP.slave[i].IP));
            }
            setTable = false;
            modbusTCP.loadTable = true;
        }
    }
}
/*********************************************END MODBUS TASK*************************************************************/

// Serial.println("Master Read Data");
// for (long i = 0; i < (modbusRTU.slave[RTUSlaveCount].ReadAddress.endAddress - modbusRTU.slave[RTUSlaveCount].ReadAddress.startAddress + 1); i++)
// {
//     Serial.print(Master_ReadData[i]);
//     Serial.print("|");
// }
// Serial.println();

// Serial.println("Client Read Data");
// for (long i = 0; i < (modbusTCP.slave[TCPSlaveCount].ReadAddress.endAddress - modbusTCP.slave[TCPSlaveCount].ReadAddress.startAddress + 1); i++)
// {
//     Serial.print(Client_ReadData[i]);
//     Serial.print("|");
// }
// Serial.println();