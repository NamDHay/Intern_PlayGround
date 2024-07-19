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
    uint8_t master;
    uint8_t numSlave;
    QueueHandle_t qUpdateTable;
    struct Config_t
    {
        unsigned long baud;
        HardwareSerial *port;
    } config;
    struct Address_t
    {
        long startAddress;
        long endAddress;
    };
    struct Slave_t
    {
        byte ID;
        Address_t WriteAddress;
        Address_t ReadAddress;
    };
    Slave_t slave[20];
    uint8_t typeData[200];

    void MasterInit(HardwareSerial *port, unsigned long baud);
    void SlaveInit(HardwareSerial *port, unsigned long baud);
    bool read_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length);
    bool write_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length);
    void loadSetting();
    void writeSetting();
    void loadSlave();
    void writeSlave();
};

class MODBUS_TCP
{
public:
    uint8_t client;
    struct ethernet_t
    {
        String ipAdress;
        String gateway;
        String subnet;
        String primaryDNS;
    };
    ethernet_t ethernet;
    String remote;
    struct Address_t
    {
        long startAddress;
        long endAddress;
    };
    struct Slave_t
    {
        String IP;
        Address_t WriteAddress;
        Address_t ReadAddress;
    };
    Slave_t slave[20];

    Address_t ServerWriteReg;
    Address_t ServerReadReg;
    Address_t ClientWriteReg;
    Address_t ClientReadReg;

    uint8_t typeData[200];

    void EthernetInit();
    void ClientInit();
    void ServerInit();
    bool read_Multiple_Data(IPAddress ID, uint16_t *value, long startAddress, size_t length);
    bool write_Multiple_Data(IPAddress ID, uint16_t *value, long startAddress, size_t length);
    void loadSetting();
    void writeSetting();
    void loadSlave();
    void writeSlave();
};
void TaskModbus(void *pvParameter);

extern MODBUS_RTU modbusRTU;
extern MODBUS_TCP modbusTCP;

#endif //__MODBUS_RTU_H__