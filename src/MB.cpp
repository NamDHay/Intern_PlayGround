#include <OnlineManage.h>
#include <MB.h>
#include <ArduinoJson.h>
#include <File_System.h>

ModbusRTU mb;

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

    // String baud = doc["baud"].as<String>();
    // String readStart = doc["readStart"].as<String>();
    // String readEnd = doc["readEnd"].as<String>();
    // String writeStart = doc["writeStart"].as<String>();
    // String writeEnd = doc["writeEnd"].as<String>();
    // String serial = doc["serial"].as<String>();
    // String mbmaster = doc["mbmaster"].as<String>();

    unsigned long baud = doc["baud"];
    long readStart = doc["readStart"];
    long readEnd = doc["readEnd"];
    long writeStart = doc["writeStart"];
    long writeEnd = doc["writeEnd"];
    String serial = doc["serial"];
    String mbmaster = doc["mbmaster"];

    modbus.master = (mbmaster == "0") ? 0 : 1;
    modbus.config.baud = baud;
    modbus.config.port = (serial == "0") ? &Serial1 : &Serial2;
    if (modbus.master == 1)
    {
        modbus.MasterReadReg.startAddress = readStart;
        modbus.MasterReadReg.endAddress = readEnd;
        modbus.MasterWriteReg.startAddress = writeStart;
        modbus.MasterWriteReg.endAddress = writeEnd;
    }

    if (modbus.master == 0)
    {
        modbus.SlaveReadReg.startAddress = readStart;
        modbus.SlaveReadReg.endAddress = readEnd;
        modbus.SlaveWriteReg.startAddress = writeStart;
        modbus.SlaveWriteReg.endAddress = writeEnd;
    }

    // Serial.println("baudrate: " + String(modbus.config.baud));
    // Serial.println("Readstart: " + String(modbus.MasterReadReg.startAddress));
    // Serial.println("Readend: " + String(modbus.MasterReadReg.endAddress));
    // Serial.println("WriteStart: " + String(modbus.MasterWriteReg.startAddress));
    // Serial.println("WriteEnd: " + String(modbus.MasterWriteReg.endAddress));
    // Serial.println("Serial: " + String(serial));
    // Serial.println("MBmaster: " + String(mbmaster));

    Serial.print("baudrate: ");
    Serial.println(modbus.config.baud);
    Serial.print("Readstart: ");
    Serial.println((modbus.MasterReadReg.startAddress));
    Serial.print("Readend: ");
    Serial.println((modbus.MasterReadReg.endAddress));
    Serial.print("WriteStart: ");
    Serial.println((modbus.MasterWriteReg.startAddress));
    Serial.print("WriteEnd: ");
    Serial.println((modbus.MasterWriteReg.endAddress));
    Serial.print("Serial: ");
    Serial.println((serial));
    Serial.print("MBmaster: ");
    Serial.println((mbmaster));
}
void MODBUS_RTU::writeSetting()
{
    String setting = "";
    JsonDocument writeDoc;

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
        for (uint8_t i = rstart; i < rstart + 30; i++)
        {
            Serial.print("Data " + String(i) + ": ");
            Serial.println(rdata[i]);
        }
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

bool update_WebData_Interval(int16_t *Master_ReadData)
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
    JsonDocument wDoc;
    JsonDocument mbOjb;
    String fbDataString = "";
    JsonArray dataArray;
    long Address;
    if (online.isConnected == true)
    {
        wDoc["Command"] = "Data";
        dataArray = wDoc["Data"].to<JsonArray>();
        mbOjb["slaveID"] = modbus.config.slaveID;

        Address = modbus.MasterReadReg.startAddress;
        for (uint8_t i = 0; i < (modbus.MasterReadReg.endAddress - modbus.MasterReadReg.startAddress + 1); i++)
        {
            if (Address > modbus.MasterReadReg.endAddress)
            {
                Serial.println("Out of range");
                break;
            }
            mbOjb["address"] = Address;
            mbOjb["type"] = modbus.mbdata[i].typeData;
            switch (modbus.mbdata[i].typeData)
            {
            case COIL_TYPE:
                mbOjb["value"] = CHECKCOIL(Master_ReadData[Address], 0);
                Address++;
                break;
            case WORD_TYPE:
                mbOjb["value"] = Master_ReadData[Address];
                Address += 1;
                break;
            case DWORDS_TYPE:
                dw2w.w[0] = Master_ReadData[Address];
                dw2w.w[1] = Master_ReadData[Address + 1];
                mbOjb["value"] = dw2w.dw;
                Address += 2;
                break;
            case FLOAT_TYPE:
                f2w.w[0] = Master_ReadData[Address];
                f2w.w[1] = Master_ReadData[Address + 1];
                mbOjb["value"] = f2w.f;
                Address += 2;
                break;
            default:
                break;
            }
            dataArray.add(mbOjb);
        }
        serializeJson(wDoc, fbDataString);
        online.notifyClients(fbDataString);
        return true;
    }
}

void TaskModbus(void *pvParameter)
{
    int16_t *Master_ReadData;

    modbus.master = 1;
    modbus.config.slaveID = 1;
    modbus.MasterReadReg.startAddress = 0;
    modbus.MasterReadReg.endAddress = 19;
    modbus.MasterWriteReg.startAddress = 20;
    modbus.MasterWriteReg.endAddress = 39;
    size_t reglengh = (modbus.MasterReadReg.endAddress - modbus.MasterReadReg.startAddress + 1);
    // modbus.mbdata[0].typeData = COIL_TYPE;
    // modbus.mbdata[1].typeData = WORD_TYPE;
    // modbus.mbdata[2].typeData = COIL_TYPE;
    // modbus.mbdata[3].typeData = DWORDS_TYPE;
    // modbus.mbdata[4].typeData = FLOAT_TYPE;
    // modbus.mbdata[5].typeData = COIL_TYPE;
    // modbus.mbdata[6].typeData = WORD_TYPE;
    // modbus.mbdata[7].typeData = COIL_TYPE;
    // modbus.mbdata[8].typeData = DWORDS_TYPE;
    // modbus.mbdata[9].typeData = COIL_TYPE;

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
            Master_ReadData = (int16_t *)malloc(reglengh * sizeof(int16_t));

            if (!mb.slave())
            {
                while (modbus.read_Multiple_Data(modbus.config.slaveID,
                                                 (uint16_t *)&Master_ReadData,
                                                 modbus.MasterReadReg.startAddress,
                                                 (modbus.MasterReadReg.endAddress - modbus.MasterReadReg.startAddress + 1)) != true)
                    ;
            }
            if (millis() - startUpdateIntervalTime >= UPDATE_INTERVAL_MS)
            {
                startUpdateIntervalTime = millis();
                while (update_WebData_Interval(Master_ReadData) != true)
                    ;
                Serial.println("update_WebData_Interval success");
                Serial.println("Free data");
                free(Master_ReadData);
            }
        } // Master part
    }
}
