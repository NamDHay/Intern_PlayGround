#include <OnlineManage.h>
#include <MB.h>

ModbusRTU mb;
#define MODBUS_BAUD_ 9600

bool cb(Modbus::ResultCode event, uint16_t transactionId, void *data)
{ // Callback to monitor errors
    if (event != Modbus::EX_SUCCESS)
    {
        Serial.print("Request result: 0x");
        Serial.print(event, HEX);
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
    static uint16_t start;
    static long dataLength = 0;
    static uint16_t *rdata = NULL;
    if (!length || !value)
        return false;
    if (!dataLength || !rdata)
    {
        start = startAddress;
        dataLength = length;
        rdata = value;
    }
    if (dataLength >= 30)
    {
        mb.readHreg(ID,
                    start,
                    rdata,
                    start + 30 - 1,
                    cb);
        while (mb.slave())
        {
            mb.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        start = start + 30 - 1;
        dataLength -= 30;
        
        return false;;
    }
    else if (dataLength < 30)
    {
        mb.readHreg(ID,
                    start,
                    rdata,
                    start + dataLength - 1,
                    cb);
        while (mb.slave())
        {
            mb.task();
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        dataLength = 0;
        start = 0;
        rdata = NULL;
        return true;
    }
    return false;
}

void TaskModbus(void *pvParameter)
{
    uint8_t master = 0;
    uint16_t rvalue[60];
    uint16_t wvalue[60];
    for (int i = 0; i < 60; i++)
    {
        wvalue[i] = i;
    }
    modbus.config.slaveID = 0x01;
    modbus.readTemp.startAddress = 0;
    modbus.readTemp.endAddress = 60;
    modbus.writeTemp.startAddress = 0;
    modbus.writeTemp.endAddress = 60;
    if (master == 1)
    {
        modbus.MasterInit(&Serial2, MODBUS_BAUD_);
    }
    else
    {
        modbus.SlaveInit(&Serial2, MODBUS_BAUD_);
        mb.slave(modbus.config.slaveID);
        mb.addHreg(modbus.writeTemp.startAddress, 0, 30);
    }
    for (;;)
    {
        if (master == 1) // Master part
        {
            if (!mb.slave())
            {
                mb.readHreg(modbus.config.slaveID,
                            modbus.readTemp.startAddress,
                            rvalue,
                            modbus.readTemp.endAddress - modbus.readTemp.startAddress + 1,
                            cb);
                while (mb.slave())
                {
                    mb.task();
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                }
                Serial.println("\nSlaveID: " + String(modbus.config.slaveID));
                for (uint8_t i = modbus.readTemp.startAddress; i <= modbus.readTemp.endAddress; i++)
                {
                    Serial.print("Data" + String(i) + ": ");
                    Serial.println(rvalue[i - modbus.readTemp.startAddress]);
                    Serial.flush();
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                }
            } // Master part
        }
        else // Slave part
        {
            Serial.println("SlaveID: " + String(modbus.config.slaveID));
            for (uint8_t i = modbus.writeTemp.startAddress; i <= modbus.writeTemp.endAddress; i++)
            {
                mb.Hreg(i, wvalue[i]);
                Serial.print("Data" + String(i) + ": ");
                Serial.println(wvalue[i]);
                Serial.flush();
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            mb.task();
        } // Slave part
    }
}
