#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__
#include <Arduino.h>
#include <ETH.h>
#include <ModbusIP_ESP8266.h>
#include <ModbusRTU.h>

class MODBUS_PARAMETER
{
public:
    bool loadTable;
    uint8_t numSlave;
    struct Address_t
    {
        long startAddress;
        long endAddress;
    };
    struct Slave_t
    {
        uint16_t Data[200];
        Address_t ReadAddress;
        String ID;
        bool IsConnected;
    };
    Slave_t slave[5];
    void loadSlave();
    void writeSlave();
};
extern MODBUS_PARAMETER mbParam;
class MODBUS_RTU
{
public:
    bool isConnected;
    uint8_t master;
    struct Config_t
    {
        unsigned long baud;
        HardwareSerial *port;
    } config;

    void MasterInit(HardwareSerial *port = &Serial2, unsigned long baud = 9600);
    bool read_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length);
    bool write_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length);
    void loadSetting();
    void writeSetting();
};

class MODBUS_TCP
{
public:
    bool isConnected;
    uint8_t client;
    struct ethernet_t
    {
        String ipAdress;
        String gateway;
        String subnet;
        String primaryDNS;
    };
    ethernet_t ethernet;

    IPAddress str2IP(String str);
    void EthernetInit();
    void ClientInit();
    bool read_Multiple_Data(IPAddress ID, uint16_t *value, long startAddress, size_t length);
    bool write_Multiple_Data(IPAddress ID, uint16_t *value, long startAddress, size_t length);
    void loadSetting();
    void writeSetting();
};

void TaskModbus(void *pvParameter);

extern MODBUS_RTU modbusRTU;
extern MODBUS_TCP modbusTCP;

#endif //__MODBUS_RTU_H__
