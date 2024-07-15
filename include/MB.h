#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__

#include <ModbusRTU.h>

class MODBUS_RTU
{
private:
public:
    bool master;
    struct Config_t
    {
        byte slaveID;
        unsigned long baud;
        HardwareSerial *port;
    } config;
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

    // enum TypeData_t
    // {
    //     COIL,
    //     WORD,
    //     DWORDS,
    //     FLOAT,
    // };
    struct modbus_data_t
    {
        long address;
        // TypeData_t typeData;
        uint8_t typeData;
    };
    struct modbus_data_t mbdata[10];
    void MasterInit(HardwareSerial *port, unsigned long baud);
    void SlaveInit(HardwareSerial *port, unsigned long baud);

    void loadSetting();
    void writeSetting();
};
void TaskModbus(void *pvParameter);

extern MODBUS_RTU modbus;

#endif //__MODBUS_RTU_H__
