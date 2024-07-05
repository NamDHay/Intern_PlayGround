#include <MB.h>

ModbusRTU mb;

bool cbWrite(Modbus::ResultCode event, uint16_t transactionId, void* data) {
    Serial.printf_P("Request result: 0x%02X, Mem: %d\n", event, ESP.getFreeHeap());
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

bool coils[20];
void TaskModbus(void *pvParameter) {
    for(;;){
        if (!mb.slave()) {
            mb.readCoil(1, 1, coils, 20, cbWrite);
        }
        mb.task();
        yield();
    }
}
