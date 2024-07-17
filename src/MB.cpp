#include <OnlineManage.h>
#include <MB.h>
#include <ArduinoJson.h>
#include <File_System.h>

ModbusRTU mbRTU;
ModbusIP mbTCP;
bool EthernetAvilable;

int16_t Master_ReadData[200];
int16_t Master_WriteData[200];
int16_t Slave_ReadData[200];
int16_t Slave_WriteData[200];

int16_t Client_ReadData[200];
int16_t Client_WriteData[200];
int16_t Server_ReadData[200];
int16_t Server_WriteData[200];

#define MODBUS_BAUD_ 9600

#define WORD_TYPE 0
#define COIL_TYPE 1
#define DWORDS_TYPE 2
#define FLOAT_TYPE 3

#define ETH_ADDR 1
#define ETH_POWER_PIN -1 // Do not use it, it can cause conflict during the software reset.
#define ETH_POWER_PIN_ALTERNATIVE 14
#define ETH_MDC_PIN 23
#define ETH_MDIO_PIN 18
#define ETH_TYPE ETH_PHY_LAN8720
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN

#define CHECKCOIL(CoilGroup, CoilBit) ((((CoilGroup) & (1UL << CoilBit)) == (1UL << CoilBit)) ? 1 : 0)
#define SETCOIL(CoilGroup, CoilBit) ((CoilGroup) |= (1UL << CoilBit))
#define CLEARCOIL(CoilGroup, CoilBit) (CoilGroup &= ~(1UL << CoilBit))
/*********************************************START RTU PART**************************************************************/
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
    if (dataLength >= 30)
    {
        mbRTU.readHreg(ID,
                    rstart,
                    rdata,
                    30,
                    cb);
        while (mbRTU.slave())
        {
            mbRTU.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        // for (uint8_t i = rstart; i < rstart + 30; i++)
        // {
        //     Serial.print("Data " + String(i) + ": ");
        //     Serial.println(rdata[i]);
        // }
        rstart = rstart + 30;
        dataLength -= 30;
        rdata += 30;
        if (dataLength == 0)
        {
            return true;
        }
        return false;
    }
    else if (dataLength < 30)
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
    if (wdataLength >= 30)
    {
        mbRTU.writeHreg(ID,
                     wstart,
                     wdata,
                     30,
                     cb);
        while (mbRTU.slave())
        {
            mbRTU.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        wstart = wstart + 30;
        wdataLength -= 30;
        wdata += 30;
        if (wdataLength == 0)
        {
            return true;
        }
        return false;
    }
    else if (wdataLength < 30)
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



/*********************************************END TCP PART****************************************************************/




void TaskModbus(void *pvParameter)
{
    modbusRTU.loadSetting();
    bool setTable = false;
    modbusRTU.loadTable = true;
    size_t reglengh = (modbusRTU.MasterReadReg.endAddress - modbusRTU.MasterReadReg.startAddress + 1);
    modbusRTU.qUpdateTable = xQueueCreate(1, sizeof(bool));
    static long startUpdateIntervalTime = millis();

    if (modbusRTU.master == 1)
    {
        modbusRTU.MasterInit(modbusRTU.config.port, modbusRTU.config.baud);
    }
    else
    {
        modbusRTU.SlaveInit(modbusRTU.config.port, modbusRTU.config.baud);
        mbRTU.slave(modbusRTU.config.slaveID);
        mbRTU.addHreg(modbusRTU.SlaveWriteReg.startAddress, 0, modbusRTU.SlaveWriteReg.endAddress - modbusRTU.SlaveWriteReg.startAddress);
        mbRTU.addHreg(modbusRTU.SlaveReadReg.startAddress, 0, modbusRTU.SlaveReadReg.endAddress - modbusRTU.SlaveReadReg.startAddress);
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
                // Serial.println("Free RAM: " + String(ESP.getFreeHeap() / 1024) + "Kb");
                // Serial.println("Current RAM: " + String((ESP.getHeapSize() - ESP.getFreeHeap()) / 1024) + "Kb");
            }
            if (xQueueReceive(modbusRTU.qUpdateTable, (void *)&setTable, 1 / portTICK_PERIOD_MS) == pdTRUE || (modbusRTU.loadTable == true && online.isConnected == true))
            {
                modbusRTU.update_WebTable();
                setTable = false;
                modbusRTU.loadTable = false;
            }

        } // Master part
    }
}
