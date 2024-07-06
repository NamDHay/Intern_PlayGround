#include <OnlineManage.h>
#include <MB.h>

ModbusRTU mb;

bool cb(Modbus::ResultCode event, uint16_t transactionId, void *data) { // Callback to monitor errors
    if (event != Modbus::EX_SUCCESS) {
        Serial.print("Request result: 0x");
        Serial.print(event, HEX);
    }
    return true;
}

void MODBUS_RTU::MasterInit(HardwareSerial *port, unsigned long baud) {
    modbus.config.port = port;
    modbus.config.baud = baud;
    port->begin(baud);
    mb.begin(port);
    mb.master();
}

void MODBUS_RTU::SlaveInit(HardwareSerial *port, unsigned long baud) {
    modbus.config.port = port;
    modbus.config.baud = baud;
    port->begin(baud);
    mb.begin(port);
}

void TaskModbus(void *pvParameter) {
    bool master = true;
    uint16_t *rvalue;
    uint16_t *wvalue;
    for (;;) {
        if (master) {
            if (!mb.slave()) {
                mb.readHreg(modbus.config.slaveID,
                            modbus.readTemp.startAddress,
                            rvalue,
                            modbus.readTemp.startAddress - modbus.readTemp.startAddress,
                            cb);
                while (mb.slave()) {
                    mb.task();
                }
            }
        }
        else {
            mb.slave(modbus.config.slaveID);
            mb.writeHreg(modbus.config.slaveID,
                         modbus.writeTemp.startAddress,
                         wvalue,
                         modbus.writeTemp.startAddress - modbus.writeTemp.startAddress,
                         cb);
            mb.task();
            yield();
        }
    }
}
