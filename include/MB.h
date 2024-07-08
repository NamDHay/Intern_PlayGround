#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__

#include <ModbusRTU.h>

class MODBUS_RTU {
private:   
public:
    bool mbmaster;
    struct Config_t{
        byte slaveID;
        unsigned long baud;
        HardwareSerial *port;
    }config;
    struct Read_t {
        long startAddress;
        long endAddress;
    }readTemp;
    struct Write_t {
        long startAddress;
        long endAddress;
    }writeTemp;
    void MasterInit(HardwareSerial *port, unsigned long baud);
    void SlaveInit(HardwareSerial *port, unsigned long baud);
};
void TaskModbus(void *pvParameter);

extern MODBUS_RTU modbus;

#endif //__MODBUS_RTU_H__
