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
    struct MRead_t {
        long startAddress;
        long endAddress;
    }MasterReadReg;
    struct MWrite_t {
        long startAddress;
        long endAddress;
    }MasterWriteReg;


    struct SRead_t {
        long startAddress;
        long endAddress;
    }SlaveReadReg;
    struct SWrite_t {
        long startAddress;
        long endAddress;
    }SlaveWriteReg;
    void MasterInit(HardwareSerial *port, unsigned long baud);
    void SlaveInit(HardwareSerial *port, unsigned long baud);
};
void TaskModbus(void *pvParameter);

extern MODBUS_RTU modbus;

#endif //__MODBUS_RTU_H__
