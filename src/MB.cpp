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

/*********************************************START RTU PART**************************************************************/
#define CHECKCOIL(CoilGroup, CoilBit) ((((CoilGroup) & (1UL << CoilBit)) == (1UL << CoilBit)) ? 1 : 0)
#define SETCOIL(CoilGroup, CoilBit) ((CoilGroup) |= (1UL << CoilBit))
#define CLEARCOIL(CoilGroup, CoilBit) (CoilGroup &= ~(1UL << CoilBit))

ModbusRTU mbRTU;
#define MODBUS_BAUD_ 9600

int16_t Master_ReadData[200];
int16_t Master_WriteData[200];
int16_t Slave_ReadData[200];
int16_t Slave_WriteData[200];

bool cb(Modbus::ResultCode event, uint16_t transactionId, void *data)
{ // Callback to monitor errors
    if (event != Modbus::EX_SUCCESS)
    {
        Serial.print("Request result: 0x");
        Serial.println(event, HEX);
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
    dataRead = filesystem.readfile("/mbsetting.json");
    deserializeJson(doc, dataRead);

    String slaveID = doc["slaveID"].as<String>();
    String baud = doc["baud"].as<String>();
    String readStart = doc["readStart"].as<String>();
    String readEnd = doc["readEnd"].as<String>();
    String writeStart = doc["writeStart"].as<String>();
    String writeEnd = doc["writeEnd"].as<String>();
    String serial = doc["serial"].as<String>();
    String mbmaster = doc["mbmaster"].as<String>();

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

    modbusRTU.writeSetting();
}
void MODBUS_RTU::writeSetting()
{
    String setting = "";
    JsonDocument writeDoc;

    writeDoc["slaveID"] = modbusRTU.config.slaveID;
    writeDoc["baud"] = modbusRTU.config.baud;
    writeDoc["readStart"] = modbusRTU.MasterReadReg.startAddress;
    writeDoc["readEnd"] = modbusRTU.MasterReadReg.endAddress;
    writeDoc["writeStart"] = modbusRTU.MasterWriteReg.startAddress;
    writeDoc["writeEnd"] = modbusRTU.MasterWriteReg.endAddress;
    writeDoc["serial"] = (modbusRTU.config.port == &Serial1) ? "0" : "1";
    writeDoc["mbmaster"] = modbusRTU.master;

    serializeJson(writeDoc, setting);
    Serial.println("JSON: " + setting);
    filesystem.writefile("/mbsetting.json", setting, 0);
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
        // for (uint8_t i = rstart; i < rstart + RTU_MAX_RW; i++)
        // {
        //     Serial.print("Data " + String(i) + ": ");
        //     Serial.println(rdata[i]);
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
                       dataLength,
                       cb);
        while (mbRTU.slave())
        {
            mbRTU.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
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
void MODBUS_RTU::update_WebTable()
{
    JsonDocument Doc;
    String fbDataString = "";
    long Address;
    if (online.isConnected == true)
    {
        Doc["Command"] = "TableID";
        Address = modbusRTU.MasterReadReg.startAddress;
        for (uint8_t i = 0; i < (modbusRTU.MasterReadReg.endAddress - modbusRTU.MasterReadReg.startAddress + 1); i++)
        {
            Doc["Data"][i]["slaveID"] = modbusRTU.config.slaveID;
            if (Address > modbusRTU.MasterReadReg.endAddress)
            {
                Serial.println("Out of range");
                break;
            }
            Doc["Data"][i]["address"] = Address;
            Doc["Data"][i]["type"] = modbusRTU.typeData[i];
            switch (modbusRTU.typeData[i])
            {
            case COIL_TYPE:
                Address++;
                break;
            case WORD_TYPE:
                Address += 1;
                break;
            case DWORDS_TYPE:
                Address += 2;
                break;
            case FLOAT_TYPE:
                Address += 2;
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
void MODBUS_RTU::update_WebData_Interval() // Must be void function to avoid control reaches end of non-void function [-Wreturn-type]
{
    union f2w_t
    {
        float f;
        int16_t w[2];
    } f2w;
    union dw2w_t
    {
        int32_t dw;
        int16_t w[2];
    } dw2w;
    JsonDocument Doc; // this guy make error messages if not return void
    String fbDataString = "";
    long Address;
    if (online.isConnected == true)
    {
        Doc["Command"] = "tableData";
        Address = modbusRTU.MasterReadReg.startAddress;
        for (uint8_t i = 0; i < (modbusRTU.MasterReadReg.endAddress - modbusRTU.MasterReadReg.startAddress + 1); i++)
        {
            if (Address > modbusRTU.MasterReadReg.endAddress)
            {
                Serial.println("Out of range");
                break;
            }
            switch (modbusRTU.typeData[i])
            {
            case COIL_TYPE:
                Doc["Data"][i] = CHECKCOIL(Master_ReadData[Address], 0);
                Address++;
                break;
            case WORD_TYPE:
                Doc["Data"][i] = Master_ReadData[Address];
                Address += 1;
                break;
            case DWORDS_TYPE:
                dw2w.w[0] = Master_ReadData[Address];
                dw2w.w[1] = Master_ReadData[Address + 1];
                Doc["Data"][i] = dw2w.dw;
                Address += 2;
                break;
            case FLOAT_TYPE:
                f2w.w[0] = Master_ReadData[Address];
                f2w.w[1] = Master_ReadData[Address + 1];
                Doc["Data"][i] = f2w.f;
                Address += 2;
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
ModbusIP mbTCP;

String _ip = "192.168.137.3";
String _gw = "192.168.137.1";
String _sn = "255.255.255.0";
String dns1 = "8.8.8.8";
String remote = "192.168.137.2"; // Address of Modbus Slave device

int16_t Client_ReadData[200];
int16_t Client_WriteData[200];
int16_t Server_ReadData[200];
int16_t Server_WriteData[200];

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
        for (uint8_t i = rstart; i < rstart + TCP_MAX_RW; i++)
        {
            Serial.print("Data " + String(i) + ": ");
            Serial.println(rdata[i]);
        }
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
        for (uint8_t i = rstart; i < rstart + dataLength; i++)
        {
            Serial.print("Data " + String(i) + ": ");
            Serial.println(rdata[i]);
        }
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
                        TCP_MAX_RW,
                        cb);
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
}
void MODBUS_TCP::writeSetting()
{
}
void MODBUS_TCP::update_WebTable()
{
}
void MODBUS_TCP::update_WebData_Interval()
{
}
/*********************************************END TCP PART****************************************************************/

/*********************************************START MODBUS TASK***********************************************************/
void TaskModbus(void *pvParameter)
{
    // modbusRTU.loadSetting();
    // modbusTCP.loadSetting();
    /*Start Test Part*/
    modbusTCP.client = 1;
    modbusRTU.master = 1;

    modbusRTU.MasterReadReg.startAddress = 0;
    modbusRTU.MasterReadReg.endAddress = 99;
    modbusRTU.MasterWriteReg.startAddress = 100;
    modbusRTU.MasterWriteReg.endAddress = 199;
    modbusRTU.SlaveReadReg.startAddress = 0;
    modbusRTU.SlaveReadReg.endAddress = 99;
    modbusRTU.SlaveWriteReg.startAddress = 100;
    modbusRTU.SlaveWriteReg.endAddress = 199;

    modbusTCP.ethernet.ipAdress = "192.168.137.3";
    modbusTCP.ethernet.gateway = "192.168.137.1";
    modbusTCP.ethernet.subnet = "255.255.255.0";
    modbusTCP.ethernet.primaryDNS = "8.8.8.8";
    modbusTCP.remote = "192.168.137.2";

    modbusRTU.config.port = &Serial2;
    modbusRTU.config.baud = 9600;

    modbusTCP.ClientReadReg.startAddress = 0;
    modbusTCP.ClientReadReg.endAddress = 99;
    modbusTCP.ClientWriteReg.startAddress = 100;
    modbusTCP.ClientWriteReg.endAddress = 199;
    /*End Test Part*/

    bool setTable = false;
    modbusRTU.loadTable = true;

    size_t reglengh = (modbusRTU.MasterReadReg.endAddress - modbusRTU.MasterReadReg.startAddress + 1);
    modbusRTU.qUpdateTable = xQueueCreate(1, sizeof(bool));

    modbusTCP.EthernetInit();

    static long startUpdateIntervalTime = millis();
    if (modbusRTU.master == 1)
    {
        Serial.println("Start init RTU");
        modbusRTU.MasterInit(modbusRTU.config.port, modbusRTU.config.baud);
        Serial.println("Finish init RTU");
    }
    else
    {
        modbusRTU.SlaveInit(modbusRTU.config.port, modbusRTU.config.baud);
        mbRTU.slave(modbusRTU.config.slaveID);
        mbRTU.addHreg(modbusRTU.SlaveWriteReg.startAddress, 0, modbusRTU.SlaveWriteReg.endAddress - modbusRTU.SlaveWriteReg.startAddress);
        mbRTU.addHreg(modbusRTU.SlaveReadReg.startAddress, 0, modbusRTU.SlaveReadReg.endAddress - modbusRTU.SlaveReadReg.startAddress);
    }
    Serial.println("Init TCP");
    if (modbusTCP.client == 1)
    {
        modbusTCP.ClientInit();
    }
    else
    {
        modbusTCP.ServerInit();
    }
    for (;;)
    {
        if (modbusRTU.master == 1) // Master part
        {
            if (!mbRTU.slave())
            {
                while (modbusRTU.read_Multiple_Data(modbusRTU.config.slaveID,
                                                    (uint16_t *)&Master_ReadData,
                                                    modbusRTU.MasterReadReg.startAddress,
                                                    (modbusRTU.MasterReadReg.endAddress - modbusRTU.MasterReadReg.startAddress + 1)) != true)
                    ;
                // while (modbusRTU.write_Multiple_Data(modbusRTU.config.slaveID,
                //                                   (uint16_t *)&Master_ReadData,
                //                                   modbusRTU.MasterWriteReg.startAddress,
                //                                   (modbusRTU.MasterWriteReg.endAddress - modbusRTU.MasterWriteReg.startAddress + 1)) != true)
                //     ;
            }
            if (millis() - startUpdateIntervalTime >= UPDATE_INTERVAL_MS)
            {
                startUpdateIntervalTime = millis();
                modbusRTU.update_WebData_Interval();
            }
            if (xQueueReceive(modbusRTU.qUpdateTable, (void *)&setTable, 1 / portTICK_PERIOD_MS) == pdTRUE || (modbusRTU.loadTable == true && online.isConnected == true))
            {
                modbusRTU.update_WebTable();
                setTable = false;
                modbusRTU.loadTable = false;
            }

        } // Master part

        if (modbusTCP.client == 1) // Client part
        {
            if (mbTCP.isConnected(str2IP(modbusTCP.remote)))
            {
                while (modbusTCP.read_Multiple_Data(str2IP(modbusTCP.remote),
                                                    (uint16_t *)&Master_ReadData,
                                                    modbusTCP.ClientReadReg.startAddress,
                                                    (modbusTCP.ClientReadReg.endAddress - modbusTCP.ClientReadReg.startAddress + 1)) != true)
                    ;
                while (modbusTCP.write_Multiple_Data(str2IP(modbusTCP.remote),
                                                     (uint16_t *)&Master_ReadData,
                                                     modbusTCP.ClientWriteReg.startAddress,
                                                     (modbusTCP.ClientWriteReg.endAddress - modbusTCP.ClientWriteReg.startAddress + 1)) != true)
                    ;
            }
            else
            {
                mbTCP.connect(str2IP(modbusTCP.remote));
            }
            mbTCP.task();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } // Client part
    }
}
/*********************************************END MODBUS TASK*************************************************************/
