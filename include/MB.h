#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__

#include <SPI.h>
#include <ETH.h>
#include <ModbusIP_ESP8266.h>
#include <ModbusRTU.h>

class MODBUS_PARAMETER
{
    bool loadTable;
    uint8_t typeTable;
    byte slaveTable;
    uint8_t numSlave;
    struct Address_t
    {
        long startAddress;
        long endAddress;
    };
    struct Slave_t
    {
        Address_t WriteAddress;
        Address_t ReadAddress;
        String ID;
        uint8_t typeData[200];
    };
    Slave_t slave[255];
};
extern MODBUS_PARAMETER mbParam;
class MODBUS_RTU
{
public:
    bool loadTable;
    bool isConnected;
    uint8_t typeTable;
    byte slaveTable;
    uint8_t numSlave;
    uint8_t master;
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
        String ID;
        Address_t WriteAddress;
        Address_t ReadAddress;
        uint8_t typeData[200];
    };
    Slave_t slave[20];

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
    bool loadTable;
    bool isConnected;
    uint8_t typeTable;
    byte slaveTable;
    uint8_t numSlave;
    uint8_t client;
    QueueHandle_t qUpdateTable;
    struct ethernet_t
    {
        String ipAdress;
        String gateway;
        String subnet;
        String primaryDNS;
    };
    ethernet_t ethernet;
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
        uint8_t typeData[200];
    };
    Slave_t slave[20];

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
