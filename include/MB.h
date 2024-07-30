#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__

#include <SPI.h>
#include <ETH.h>
#include <ModbusIP_ESP8266.h>
#include <ModbusRTU.h>

#define WORD_TYPE 0
#define COIL_TYPE 1
#define DWORDS_TYPE 2
#define FLOAT_TYPE 3
#define CHAR_TYPE 4

#define CHECKCOIL(CoilGroup, CoilBit) ((((CoilGroup) & (1UL << CoilBit)) == (1UL << CoilBit)) ? 1 : 0)
#define SETCOIL(CoilGroup, CoilBit) ((CoilGroup) |= (1UL << CoilBit))
#define CLEARCOIL(CoilGroup, CoilBit) (CoilGroup &= ~(1UL << CoilBit))

class MODBUS_PARAMETER
{
public:
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
        uint16_t Data[200];
        Address_t WriteAddress;
        Address_t ReadAddress;
        String ID;
        bool IsConnected;
    };
    Slave_t slave[5];
    union data_t
    {
        float f;
        int32_t dw;
        uint16_t w[2];
    } write_data;
    void loadSlave();
    void writeSlave();
    void u16_to_c(char *c_arr, uint16_t *u_arr);
    void c_to_u16(char *c_arr, uint16_t *u_arr);
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
    struct Config_t
    {
        unsigned long baud;
        HardwareSerial *port;
    } config;

    void MasterInit(HardwareSerial *port, unsigned long baud);
    void SlaveInit(HardwareSerial *port, unsigned long baud);
    bool read_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length);
    bool write_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length);
    void loadSetting();
    void writeSetting();
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
    void ServerInit();
    bool read_Multiple_Data(IPAddress ID, uint16_t *value, long startAddress, size_t length);
    bool write_Multiple_Data(IPAddress ID, uint16_t *value, long startAddress, size_t length);
    void loadSetting();
    void writeSetting();
};
void TaskModbus(void *pvParameter);

extern MODBUS_RTU modbusRTU;
extern MODBUS_TCP modbusTCP;

#endif //__MODBUS_RTU_H__
