#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__

#include <SPI.h>
#include <ETH.h>
#include <ModbusIP_ESP8266.h>
#include <ModbusRTU.h>

class MODBUS_RTU
{
public:
    bool loadTable;
    bool master;
    QueueHandle_t qUpdateTable;
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
    uint8_t typeData[200];
    void MasterInit(HardwareSerial *port, unsigned long baud);
    void SlaveInit(HardwareSerial *port, unsigned long baud);
    bool read_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length);
    bool write_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length);
    void loadSetting();
    void writeSetting();
    void update_WebTable();
    void update_WebData_Interval();
};

class MODBUS_TCP
{
public:
    struct CRead_t
    {
        long startAddress;
        long endAddress;
    } ClientReadReg;
    struct CWrite_t
    {
        long startAddress;
        long endAddress;
    } ClientWriteReg;

    struct SRead_t
    {
        long startAddress;
        long endAddress;
    } ServerReadReg;
    struct SWrite_t
    {
        long startAddress;
        long endAddress;
    } ServerWriteReg;
};
void TaskModbus(void *pvParameter);

extern MODBUS_RTU modbusRTU;
extern MODBUS_TCP modbusTCP;

#endif //__MODBUS_RTU_H__
