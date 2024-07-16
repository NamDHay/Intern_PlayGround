#include <Arduino.h>
#include <ModbusRTU.h>
bool master;
ModbusRTU mb;
#define MODBUS_BAUD_ 9600

#define COIL_TYPE 0
#define WORD_TYPE 1
#define DWORDS_TYPE 2
#define FLOAT_TYPE 3

#define CHECKCOIL(CoilGroup, CoilBit) ((((CoilGroup) & (1UL << CoilBit)) == (1UL << CoilBit)) ? 1 : 0)
#define SETCOIL(CoilGroup, CoilBit) ((CoilGroup) |= (1UL << CoilBit))
#define CLEARCOIL(CoilGroup, CoilBit) (CoilGroup &= ~(1UL << CoilBit))

int16_t Master_ReadData[60];
int16_t Master_WriteData[60];
int16_t Slave_ReadData[60];
int16_t Slave_WriteData[60];
struct ModBusConfig
{
  unsigned long baud;
  byte slaveID;
  HardwareSerial *port;
} mbusconfig;
struct MRead_t
{
  long startAddress;
  long endAddress;
} MasterReadReg;
struct MWrite_t
{
  long startAddress;
  long endAddress;
} MasterWriteReg;

struct SRead_t
{
  long startAddress;
  long endAddress;
} SlaveReadReg;
struct SWrite_t
{
  long startAddress;
  long endAddress;
} SlaveWriteReg;

struct modbus_data_t
{
  long address;
  uint8_t typeData;
};
struct modbus_data_t mbdata[10];

bool cb(Modbus::ResultCode event, uint16_t transactionId, void *data)
{ // Callback to monitor errors
  if (event != Modbus::EX_SUCCESS)
  {
    Serial.print("Request result: 0x");
    Serial.print(event, HEX);
  }
  return true;
}
void Modbus_MasterInit(HardwareSerial *port, unsigned long baud)
{
  mbusconfig.port = port;
  mbusconfig.baud = baud;
  port->begin(baud, SERIAL_8N1);
  mb.begin(port);
  mb.master();
}
void Modbus_SlaveInit(HardwareSerial *port, unsigned long baud)
{
  mbusconfig.port = port;
  mbusconfig.baud = baud;
  port->begin(baud, SERIAL_8N1);
  mb.begin(port);
}
void Modbus_writeSetting()
{
  String setting = "";
  JsonDocument writeDoc;

  writeDoc["baud"] = mbusconfig.baud;
  writeDoc["readStart"] = MasterReadReg.startAddress;
  writeDoc["readEnd"] = MasterReadReg.endAddress;
  writeDoc["writeStart"] = MasterWriteReg.startAddress;
  writeDoc["writeEnd"] = MasterWriteReg.endAddress;
  writeDoc["serial"] = (mbusconfig.port == &Serial1) ? "0" : "1";
  writeDoc["mbmaster"] = master;

  serializeJson(writeDoc, setting);
  Serial.println("JSON: " + setting);
  writefile("/mbsetting.json", setting);
}
void Modbus_loadSetting()
{
  String dataRead = "";
  JsonDocument doc;
  dataRead = readfile("/mbsetting.json");
  deserializeJson(doc, dataRead);

  unsigned long baud = doc["baud"];
  long readStart = doc["readStart"];
  long readEnd = doc["readEnd"];
  long writeStart = doc["writeStart"];
  long writeEnd = doc["writeEnd"];
  String serial = doc["serial"];
  String mbmaster = doc["mbmaster"];

  master = (mbmaster == "0") ? 0 : 1;
  mbusconfig.baud = baud;
  mbusconfig.port = (serial == "0") ? &Serial1 : &Serial2;
  if (master == 1)
  {
    MasterReadReg.startAddress = readStart;
    MasterReadReg.endAddress = readEnd;
    MasterWriteReg.startAddress = writeStart;
    MasterWriteReg.endAddress = writeEnd;
  }

  if (master == 0)
  {
    SlaveReadReg.startAddress = readStart;
    SlaveReadReg.endAddress = readEnd;
    SlaveWriteReg.startAddress = writeStart;
    SlaveWriteReg.endAddress = writeEnd;
  }

  Serial.print("baudrate: ");
  Serial.println(mbusconfig.baud);
  Serial.print("Readstart: ");
  Serial.println((MasterReadReg.startAddress));
  Serial.print("Readend: ");
  Serial.println((MasterReadReg.endAddress));
  Serial.print("WriteStart: ");
  Serial.println((MasterWriteReg.startAddress));
  Serial.print("WriteEnd: ");
  Serial.println((MasterWriteReg.endAddress));
  Serial.print("Serial: ");
  Serial.println((serial));
  Serial.print("MBmaster: ");
  Serial.println((mbmaster));
  Modbus_writeSetting();
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
void TaskModbus(void *pvParameter)
{
  // master = 1;
  // mbusconfig.slaveID = 1;
  // MasterReadReg.startAddress = 0;
  // MasterReadReg.endAddress = 19;
  // MasterWriteReg.startAddress = 20;
  // MasterWriteReg.endAddress = 39;

  mbdata[0].typeData = COIL_TYPE;
  mbdata[1].typeData = WORD_TYPE;
  mbdata[2].typeData = COIL_TYPE;
  mbdata[3].typeData = DWORDS_TYPE;
  mbdata[4].typeData = FLOAT_TYPE;
  mbdata[5].typeData = COIL_TYPE;
  mbdata[6].typeData = WORD_TYPE;
  mbdata[7].typeData = COIL_TYPE;
  mbdata[8].typeData = DWORDS_TYPE;
  mbdata[9].typeData = COIL_TYPE;

  for (uint8_t i = 0; i < (MasterReadReg.endAddress - MasterReadReg.startAddress + 1); i++)
  {
    Master_ReadData[i] = 0;
  }
  for (uint8_t i = 0; i < (MasterWriteReg.endAddress - MasterWriteReg.startAddress + 1); i++)
  {
    Master_WriteData[i] = i;
  }

  if (master == 1)
  {
    Modbus_MasterInit(&Serial2, MODBUS_BAUD_);
  }
  else
  {
    Modbus_SlaveInit(&Serial2, MODBUS_BAUD_);
    mb.slave(mbusconfig.slaveID);
    mb.addHreg(SlaveWriteReg.startAddress, 0, SlaveWriteReg.endAddress - SlaveWriteReg.startAddress);
    mb.addHreg(SlaveReadReg.startAddress, 0, SlaveReadReg.endAddress - SlaveReadReg.startAddress);
  }

  for (;;)
  {
    if (master == 1) // Master part
    {
      if (!mb.slave())
      {
        if (!mb.slave())
        {
          while (read_Multiple_Data(mbusconfig.slaveID,
                                    (uint16_t *)&Master_ReadData,
                                    MasterReadReg.startAddress,
                                    (MasterReadReg.endAddress - MasterReadReg.startAddress + 1)) != true)
          ;
          while (write_Multiple_Data(mbusconfig.slaveID,
                                     (uint16_t *)&Master_WriteData,
                                     MasterWriteReg.startAddress,
                                     (MasterWriteReg.endAddress - MasterWriteReg.startAddress + 1)) != true)
          ;
        }
      }
    } // Master part
  }
}