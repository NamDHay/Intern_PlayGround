#include <OnlineManage.h>
#include <MB.h>
#include <ArduinoJson.h>
#include <File_System.h>

ModbusRTU mb;

int16_t Master_ReadData[100];
int16_t Master_WriteData[100];
int16_t Slave_ReadData[100];
int16_t Slave_WriteData[100];

#define MODBUS_BAUD_ 9600

#define COIL_TYPE 0
#define WORD_TYPE 1
#define DWORDS_TYPE 2
#define FLOAT_TYPE 3

#define CHECKCOIL(CoilGroup, CoilBit) ((((CoilGroup) & (1UL << CoilBit)) == (1UL << CoilBit)) ? 1 : 0)
#define SETCOIL(CoilGroup, CoilBit) ((CoilGroup) |= (1UL << CoilBit))
#define CLEARCOIL(CoilGroup, CoilBit) (CoilGroup &= ~(1UL << CoilBit))

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
    modbus.config.port = port;
    modbus.config.baud = baud;
    port->begin(baud, SERIAL_8N1);
    mb.begin(port);
    mb.master();
}

void MODBUS_RTU::SlaveInit(HardwareSerial *port, unsigned long baud)
{
    modbus.config.port = port;
    modbus.config.baud = baud;
    port->begin(baud, SERIAL_8N1);
    mb.begin(port);
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

    modbus.config.slaveID = slaveID.toInt();
    modbus.master = (mbmaster == "0") ? 0 : 1;
    modbus.config.baud = baud.toInt();
    modbus.config.port = (serial == "0") ? &Serial1 : &Serial2;
    if (modbus.master == 1)
    {
        modbus.MasterReadReg.startAddress = readStart.toInt();
        modbus.MasterReadReg.endAddress = readEnd.toInt();
        modbus.MasterWriteReg.startAddress = writeStart.toInt();
        modbus.MasterWriteReg.endAddress = writeEnd.toInt();
    }

    if (modbus.master == 0)
    {
        modbus.SlaveReadReg.startAddress = readStart.toInt();
        modbus.SlaveReadReg.endAddress = readEnd.toInt();
        modbus.SlaveWriteReg.startAddress = writeStart.toInt();
        modbus.SlaveWriteReg.endAddress = writeEnd.toInt();
    }

    modbus.writeSetting();
}
void MODBUS_RTU::writeSetting()
{
    String setting = "";
    JsonDocument writeDoc;

    writeDoc["slaveID"] = modbus.config.slaveID;
    writeDoc["baud"] = modbus.config.baud;
    writeDoc["readStart"] = modbus.MasterReadReg.startAddress;
    writeDoc["readEnd"] = modbus.MasterReadReg.endAddress;
    writeDoc["writeStart"] = modbus.MasterWriteReg.startAddress;
    writeDoc["writeEnd"] = modbus.MasterWriteReg.endAddress;
    writeDoc["serial"] = (modbus.config.port == &Serial1) ? "0" : "1";
    writeDoc["mbmaster"] = modbus.master;

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
        mb.readHreg(ID,
                    rstart,
                    rdata,
                    30,
                    cb);
        while (mb.slave())
        {
            mb.task();
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
        mb.readHreg(ID,
                    rstart,
                    rdata,
                    dataLength,
                    cb);
        while (mb.slave())
        {
            mb.task();
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
        mb.writeHreg(ID,
                     wstart,
                     wdata,
                     30,
                     cb);
        while (mb.slave())
        {
            mb.task();
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
        mb.writeHreg(ID,
                     wstart,
                     wdata,
                     wdataLength,
                     cb);
        while (mb.slave())
        {
            mb.task();
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
        Address = modbus.MasterReadReg.startAddress;
        for (uint8_t i = 0; i < (modbus.MasterReadReg.endAddress - modbus.MasterReadReg.startAddress + 1); i++)
        {
            Doc["Data"][i]["slaveID"] = modbus.config.slaveID;
            if (Address > modbus.MasterReadReg.endAddress)
            {
                Serial.println("Out of range");
                break;
            }
            Doc["Data"][i]["address"] = Address;
            Doc["Data"][i]["type"] = modbus.typeData[i];
            switch (modbus.typeData[i])
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
        Address = modbus.MasterReadReg.startAddress;
        for (uint8_t i = 0; i < (modbus.MasterReadReg.endAddress - modbus.MasterReadReg.startAddress + 1); i++)
        {
            if (Address > modbus.MasterReadReg.endAddress)
            {
                Serial.println("Out of range");
                break;
            }
            switch (modbus.typeData[i])
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

void TaskModbus(void *pvParameter)
{
    bool setTable = false;
    modbus.loadTable = true;
    modbus.master = 1;
    modbus.config.slaveID = 1;
    modbus.MasterReadReg.startAddress = 0;
    modbus.MasterReadReg.endAddress = 19;
    modbus.MasterWriteReg.startAddress = 20;
    modbus.MasterWriteReg.endAddress = 39;
    size_t reglengh = (modbus.MasterReadReg.endAddress - modbus.MasterReadReg.startAddress + 1);
    modbus.qUpdateTable = xQueueCreate(1, sizeof(bool));
    static long startUpdateIntervalTime = millis();

    if (modbus.master == 1)
    {
        modbus.MasterInit(&Serial2, MODBUS_BAUD_);
    }
    else
    {
        modbus.SlaveInit(&Serial2, MODBUS_BAUD_);
        mb.slave(modbus.config.slaveID);
        mb.addHreg(modbus.SlaveWriteReg.startAddress, 0, modbus.SlaveWriteReg.endAddress - modbus.SlaveWriteReg.startAddress);
        mb.addHreg(modbus.SlaveReadReg.startAddress, 0, modbus.SlaveReadReg.endAddress - modbus.SlaveReadReg.startAddress);
    }

    for (;;)
    {
        if (modbus.master == 1) // Master part
        {
            if (!mb.slave())
            {
                while (modbus.read_Multiple_Data(modbus.config.slaveID,
                                                 (uint16_t *)&Master_ReadData,
                                                 modbus.MasterReadReg.startAddress,
                                                 (modbus.MasterReadReg.endAddress - modbus.MasterReadReg.startAddress + 1)) != true)
                    ;
                while (modbus.write_Multiple_Data(modbus.config.slaveID,
                                                  (uint16_t *)&Master_ReadData,
                                                  modbus.MasterWriteReg.startAddress,
                                                  (modbus.MasterWriteReg.endAddress - modbus.MasterWriteReg.startAddress + 1)) != true)
                    ;
            }
            if (millis() - startUpdateIntervalTime >= UPDATE_INTERVAL_MS)
            {
                startUpdateIntervalTime = millis();
                modbus.update_WebData_Interval();
                // Serial.println("Free RAM: " + String(ESP.getFreeHeap() / 1024) + "Kb");
                // Serial.println("Current RAM: " + String((ESP.getHeapSize() - ESP.getFreeHeap()) / 1024) + "Kb");
            }
            if (xQueueReceive(modbus.qUpdateTable, (void *)&setTable, 1 / portTICK_PERIOD_MS) == pdTRUE || (modbus.loadTable == true && online.isConnected == true))
            {
                modbus.update_WebTable();
                setTable = false;
                modbus.loadTable = false;
            }

        } // Master part
    }
}
