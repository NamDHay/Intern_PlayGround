#include <OnlineManage.h>
#include <MB.h>
#include <ArduinoJson.h>
#include <File_System.h>

ModbusRTU mb;

#define MODBUS_BAUD_ 9600

uint16_t Master_ReadData[60];
uint16_t Master_WriteData[60];
uint16_t Slave_ReadData[60];
uint16_t Slave_WriteData[60];

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

bool read_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length)
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
            Serial.println(Master_ReadData[i]);
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
            Serial.println(Master_ReadData[i]);
        }
        dataLength = 0;
        rstart = 0;
        rdata = NULL;
        return true;
    }
    return false;
}

bool write_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length)
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
        for (uint8_t i = wstart; i < wstart + 30; i++)
        {
            Serial.print("Data " + String(i) + ": ");
            Serial.println(Master_WriteData[i - 60]);
        }

        wstart = wstart + 30;
        wdataLength -= 30;
        wdata += 30;
        Serial.println("Current data length: " + String(wdataLength));
        Serial.println("Next Write Start Address: " + String(wstart));
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
        for (uint8_t i = wstart; i < wstart + wdataLength; i++)
        {
            Serial.print("Data " + String(i) + ": ");
            Serial.println(Master_WriteData[i - 60]);
        }
        wdataLength = 0;
        wstart = 0;
        wdata = NULL;
        Serial.println("Current data length: " + String(wdataLength));
        Serial.println("Next Write Start Address: " + String(wstart));
        return true;
    }
    return false;
}

void update_WebData_Interval()
{
    JsonDocument wDoc;
    JsonDocument mbOjb;
    String fbDataString = "";
    JsonArray dataArray;
    if (online.isConnected == true)
    {
        wDoc["Command"] = "Data";
        dataArray = wDoc["Data"].to<JsonArray>();
        mbOjb["slaveID"] = modbus.config.slaveID;
        mbOjb["type"] = modbus.readTypeData;
        for (uint8_t i = 0; i < 10; i++)
        {
            mbOjb["value"] = Master_ReadData[i];
            dataArray.add(mbOjb);
        }
        serializeJson(wDoc, fbDataString);
        online.notifyClients(fbDataString);
    }
}

void TaskModbus(void *pvParameter)
{
    modbus.loadSetting();
    for (uint8_t i = 0; i < (modbus.MasterReadReg.endAddress - modbus.MasterReadReg.startAddress + 1); i++)
    {
        Master_ReadData[i] = 0;
    }
    for (uint8_t i = 0; i < (modbus.MasterWriteReg.endAddress - modbus.MasterWriteReg.startAddress + 1); i++)
    {
        Master_WriteData[i] = i;
    }
    modbus.config.slaveID = 1;

    static long startUpdateIntervalTime = millis();

    if (modbus.master == 1)
    {
        modbus.MasterInit(&Serial2, MODBUS_BAUD_);
        // modbus.MasterInit(&Serial2, modbus.config.baud);
    }
    else
    {
        modbus.SlaveInit(&Serial2, MODBUS_BAUD_);
        // modbus.SlaveInit(&Serial2, modbus.config.baud);
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
                if (!mb.slave())
                {
                    while (read_Multiple_Data(modbus.config.slaveID,
                                              Master_ReadData,
                                              modbus.MasterReadReg.startAddress,
                                              (modbus.MasterReadReg.endAddress - modbus.MasterReadReg.startAddress + 1)) != true)
                        ;
                    // vTaskDelay(1000 / portTICK_PERIOD_MS); // Need to wait for the master read to complete
                    while (write_Multiple_Data(modbus.config.slaveID,
                                               Master_WriteData,
                                               modbus.MasterWriteReg.startAddress,
                                               (modbus.MasterWriteReg.endAddress - modbus.MasterWriteReg.startAddress + 1)) != true)
                        ;
                    // vTaskDelay(1000 / portTICK_PERIOD_MS); // Need to wait for the master write to complete
                }
            }
            if (millis() - startUpdateIntervalTime >= UPDATE_INTERVAL_MS)
            {
                startUpdateIntervalTime = millis();
                update_WebData_Interval();
            }
        } // Master part
    }
}

void FourByteToOneByte(float value, byte *dataArray)
{
    byte *pFloat = (byte *)&value;

    for (byte i = 0; i < 4; i++)
    {
        dataArray[i] = *(pFloat + i);
    }
}

float OneByteToFourByte(byte *dataArray)
{
    float value;
    byte *pValue = (byte *)&value;

    for (byte i = 0; i < 4; i++)
    {
        *(pValue + i) = dataArray[i];
    }
    return value;
}
void TwoByteToOneByte(short value, byte *dataArray)
{
    byte *pShort = (byte *)&value;

    for (byte i = 0; i < 2; i++)
    {
        dataArray[i] = *(pShort + i);
        Serial.printf("0x%x \n", dataArray[i]);
    }
}
short OneByteToTwoByte(byte *dataArray)
{
    short value;
    byte *pValue = (byte *)value;
    for (byte i = 0; i < 2; i++)
    {
        *(pValue + i) = dataArray[i];
    }
    return value;
}
void BitsToBytes(bool *bitArray, byte *byteArray, int numBits)
{
    int numBytes = (numBits + 7) / 8; // Calculate the number of bytes needed

    for (int i = 0; i < numBytes; i++)
    {
        byteArray[i] = 0;
    }

    for (int i = 0; i < numBits; i++)
    {
        if (bitArray[i])
        {
            byteArray[i / 8] |= (1 << (i % 8));
        }
    }
}

void BytesToBits(byte *byteArray, bool *bitArray, int numBytes)
{
    for (int i = 0; i < numBytes; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            bitArray[i * 8 + j] = (byteArray[i] & (1 << j)) != 0;
        }
    }
}