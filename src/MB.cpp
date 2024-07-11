#include <OnlineManage.h>
#include <MB.h>

ModbusRTU mb;

#define MODBUS_BAUD_ 9600

uint8_t master = 1;
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
    /********Test*********/
    // for (int i = 0; i < 60; i++)
    // {
    //     Master_WriteData[i] = i;
    //     Master_ReadData[i] = 0;
    //     Slave_WriteData[i] = i;
    //     Slave_ReadData[i] = 0;
    // }
    // modbus.config.slaveID = 1;
    // modbus.MasterReadReg.startAddress = 0;
    // modbus.MasterReadReg.endAddress = 10;
    // modbus.MasterWriteReg.startAddress = 11;
    // modbus.MasterWriteReg.endAddress = 20;

    // modbus.SlaveReadReg.startAddress = 11;
    // modbus.SlaveReadReg.endAddress = 20;
    // modbus.SlaveWriteReg.startAddress = 0;
    // modbus.SlaveWriteReg.endAddress = 10;
    /********Test*********/

    if (master == 1)
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
        if (master == 1) // Master part
        {
            if (!mb.slave())
            {
                if (!mb.slave())
                {
                    Serial.println("Start reading from slave");
                    while (read_Multiple_Data(modbus.config.slaveID,
                                              Master_ReadData,
                                              modbus.MasterReadReg.startAddress,
                                              (modbus.MasterReadReg.endAddress - modbus.MasterReadReg.startAddress + 1)) != true)
                        ;
                    Serial.println("Done reading from slave");     // Need to wait for the master read to complete
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    Serial.println("Start writing to slave");
                    while (write_Multiple_Data(modbus.config.slaveID,
                                               Master_WriteData,
                                               modbus.MasterWriteReg.startAddress,
                                               (modbus.MasterWriteReg.endAddress - modbus.MasterWriteReg.startAddress + 1)) != true)
                        ;
                    Serial.println("Done writing to slave");
                    vTaskDelay(1000 / portTICK_PERIOD_MS);          // Need to wait for the master write to complete
                }
            }
        } // Master part
        else // Slave part
        {
            // Serial.println("SlaveID: " + String(modbus.config.slaveID));
            // for (uint8_t i = modbus.SlaveWriteReg.startAddress; i < modbus.SlaveWriteReg.endAddress; i++)
            // {
            //     mb.Hreg(i, Slave_WriteData[i]);
            //     vTaskDelay(200 / portTICK_PERIOD_MS);
            //     mb.Hreg(i, Slave_ReadData[i + 11]);
            //     vTaskDelay(200 / portTICK_PERIOD_MS);
            // }

            // for (uint8_t i = 0; i < modbus.SlaveWriteReg.endAddress; i++)
            // {
            //     Serial.print("Data" + String(i) + ": ");
            //     Serial.print(Slave_WriteData[i]);
            //     Serial.print(" | ");
            //     Serial.println(Slave_ReadData[i]);
            //     Serial.flush();
            // }
            // mb.task();
        } // Slave part
    }
}
