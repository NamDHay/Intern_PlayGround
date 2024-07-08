#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <ModbusRTU.h>

ModbusRTU mb;

struct ModBusConfig{
  unsigned long baud;
  byte TX_pin;
  byte RX_pin;
  byte SlaveID_Config;
  String mode;
  String port;
}mbusconfig;
struct ReadTemp{
  long startAddress;
  long endAddress;
}readTemp;
struct WriteTemp{
  long startAddress;
  long endAddress;
}writeTemp;

bool master = true;
struct Data{
  uint16_t *readvalue;
  uint16_t *writevalue;
}mbdata;

bool cb(Modbus::ResultCode event, uint16_t transactionId, void* data) { // Callback to monitor errors
  if (event != Modbus::EX_SUCCESS) {
    Serial.print("Request result: 0x");
    Serial.print(event, HEX);
  }
  return true;
}
void Modbus_MasterInit(){
  if(mbusconfig.port == "0"){
    Serial2.begin(mbusconfig.baud);
    mb.begin(&Serial2);
  }
  else if(mbusconfig.port == "1") {
    Serial1.begin(mbusconfig.baud);
    mb.begin(&Serial1);
  }
  Serial.println("Modbus Start mode Master");
}
void Modbus_SlaveInit(){
  if(mbusconfig.port == "0"){
    Serial2.begin(mbusconfig.baud);
    mb.begin(&Serial2);
  }
  else if(mbusconfig.port == "1") {
    Serial1.begin(mbusconfig.baud);
    mb.begin(&Serial1);
  }
  
  Serial.println("Modbus Start mode Slave");
}
void Modbus_loop(){
  if (master) {
    if (!mb.slave()) {
        mb.readHreg(mbusconfig.SlaveID_Config,
                    readTemp.startAddress,
                    mbdata.readvalue,
                    readTemp.startAddress - readTemp.startAddress,
                    cb);
        while (mb.slave()) {
            mb.task();
        }
    }
  }
  else {
    mb.slave(mbusconfig.SlaveID_Config);
    mb.writeHreg(mbusconfig.SlaveID_Config,
                  writeTemp.startAddress,
                  mbdata.writevalue,
                  writeTemp.startAddress - writeTemp.startAddress,
                  cb);
    mb.task();
    yield();
  }
}