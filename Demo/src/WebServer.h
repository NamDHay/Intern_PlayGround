#include <Arduino.h>
#include <ArduinoJson.h>
#include <Filesystem.h>

String dataRead = "";
String setting = "";
void writeSetting(){
    //Setting 
    JsonDocument writeDoc;
    writeDoc["SSID"] = settings.ssid;
    writeDoc["PASS"] = settings.pass;
    writeDoc["STATIC_IP"] = settings.staticip;   
    writeDoc["WEB_ADDRESS"] = settings.waddress;
    writeDoc["WEB_SUBNET"] = settings.wsubnet;
    writeDoc["WEB_GETWAY"] = settings.wgetway ;
    writeDoc["WIFI_MODE"] = settings.wifimode;
    writeDoc["CHANEL"] =  settings.chanel;

    serializeJson(writeDoc, setting);
    LOGLN("JSON: "+ setting);
    writefile("/setting.json", setting);
}
void loadSetting(){
    //load setting
    JsonDocument doc;//
    dataRead = readfile("/setting.json");
    deserializeJson(doc, dataRead);
    String SSID = doc["SSID"].as<String>();
    String PASS = doc["PASS"].as<String>();
    String STATIC_IP = doc["STATIC_IP"].as<String>(); 
    String WEB_ADDRESS = doc["WEB_ADDRESS"].as<String>(); 
    String WEB_SUBNET = doc["WEB_SUBNET"].as<String>(); 
    String WEB_GETWAY = doc["WEB_GETWAY"].as<String>(); 
    String WIFI_MODE = doc["WIFI_MODE"].as<String>(); 
    String CHANEL = doc["CHANEL"].as<String>(); 
    String SLAVE_ID = doc["SLAVE_ID"].as<String>(); 
    String BAUD = doc["BAUD"].as<String>(); 
    String RTU_TCP = doc["RTU_TCP"].as<String>(); 
    String SERIAL_PORT = doc["SERIAL_PORT"].as<String>(); 
    String MODE = doc["MODE"].as<String>(); 
    String WRITE_START = doc["WRITE_START"].as<String>(); 
    String WRITE_END = doc["WRITE_END"].as<String>(); 
    String READ_START = doc["READ_START"].as<String>(); 
    String READ_END = doc["READ_END"].as<String>(); 
    String SEND_VALUE = doc["SEND_VALUE"].as<String>();
    String RECEIVE_VALUE = doc["RECEIVE_VALUE"].as<String>();

    LOGLN("SSID: " + SSID);
    LOGLN("PASS: " + PASS);
    LOGLN("STATIC_IP: "+ STATIC_IP); 
    LOGLN("WEB_SUBNET: "+ WEB_SUBNET); 
    LOGLN("WEB_ADDRESS: "+ WEB_ADDRESS); 
    LOGLN("WEB_GETWAY: "+ WEB_GETWAY); 
    LOGLN("WIFI_MODE: "+ WIFI_MODE); 
    LOGLN("CHANEL: "+ CHANEL); 
    LOGLN("SLAVE_ID: "+ SLAVE_ID); 
    LOGLN("BAUD: "+ BAUD); 
    LOGLN("RTU_TCP: "+ RTU_TCP); 
    LOGLN("SERIAL_PORT: "+ SERIAL_PORT); 
    LOGLN("MODE: "+ MODE); 
    LOGLN("WRITE_START: "+ WRITE_START); 
    LOGLN("WRITE_END: "+ WRITE_END); 
    LOGLN("READ_START: "+ READ_START); 
    LOGLN("READ_END: "+ READ_END); 
    LOGLN("SEND_VALUE: "+ SEND_VALUE); 
    LOGLN("RECEIVE_VALUE: "+ RECEIVE_VALUE);
    settings.ssid = SSID;
    settings.pass = PASS;
    settings.staticip = STATIC_IP;
    settings.waddress = WEB_ADDRESS;
    settings.wsubnet = WEB_SUBNET;
    settings.wgetway = WEB_GETWAY;
    settings.wifimode = WIFI_MODE;
    settings.chanel = CHANEL;
    (String)mbusconfig.SlaveID_Config = SLAVE_ID;
    (String)mbusconfig.baud = BAUD;
    mbusconfig.port = SERIAL_PORT;
    (String)mbusconfig.mode = MODE;
    (String)writeTemp.startAddress = WRITE_START;
    (String)writeTemp.endAddress = WRITE_END;
    (String)readTemp.startAddress = READ_START;
    (String)readTemp.endAddress = READ_END;
    writeSetting();
}
String mbsetting = "";
String mbreading = "";
void ModbusSetting(){
     //Setting 
    JsonDocument mbwriteDoc;
    mbwriteDoc["SLAVE_ID"] =  mbusconfig.SlaveID_Config;
    mbwriteDoc["BAUD"] =  mbusconfig.baud;
    mbwriteDoc["SERIAL_PORT"] =  mbusconfig.port;
    mbwriteDoc["MODE"] =  mbusconfig.mode;
    mbwriteDoc["WRITE_START"] = writeTemp.startAddress;
    mbwriteDoc["WRITE_END"] = writeTemp.endAddress;
    mbwriteDoc["READ_START"] = readTemp.startAddress;
    mbwriteDoc["READ_END"] = readTemp.endAddress;
    // mbwriteDoc["SEND_VALUE"] = mbdata.writevalue;
    // mbwriteDoc["RECEIVE_VALUE"] = mbdata.readvalue;

    serializeJson(mbwriteDoc, mbsetting);
    LOGLN("JSON: "+ mbsetting);
    writefile("/mbsetting.json", mbsetting);
}
void ModbusReading(){
     //load setting
    JsonDocument mbdoc;//
    mbreading = readfile("/mbsetting.json");
    deserializeJson(mbdoc, dataRead);
   
    String SLAVE_ID = mbdoc["SLAVE_ID"].as<String>(); 
    String BAUD = mbdoc["BAUD"].as<String>(); 
    String RTU_TCP = mbdoc["RTU_TCP"].as<String>(); 
    String SERIAL_PORT = mbdoc["SERIAL_PORT"].as<String>(); 
    String MODE = mbdoc["MODE"].as<String>(); 
    String WRITE_START = mbdoc["WRITE_START"].as<String>(); 
    String WRITE_END = mbdoc["WRITE_END"].as<String>(); 
    String READ_START = mbdoc["READ_START"].as<String>(); 
    String READ_END = mbdoc["READ_END"].as<String>(); 
    String SEND_VALUE = mbdoc["SEND_VALUE"].as<String>();
    String RECEIVE_VALUE = mbdoc["RECEIVE_VALUE"].as<String>();

    LOGLN("SLAVE_ID: "+ SLAVE_ID); 
    LOGLN("BAUD: "+ BAUD); 
    LOGLN("RTU_TCP: "+ RTU_TCP); 
    LOGLN("SERIAL_PORT: "+ SERIAL_PORT); 
    LOGLN("MODE: "+ MODE); 
    LOGLN("WRITE_START: "+ WRITE_START); 
    LOGLN("WRITE_END: "+ WRITE_END); 
    LOGLN("READ_START: "+ READ_START); 
    LOGLN("READ_END: "+ READ_END); 

    mbusconfig.SlaveID_Config = SLAVE_ID.toInt();
    mbusconfig.baud = BAUD.toInt();
    mbusconfig.port = SERIAL_PORT;
    mbusconfig.mode = MODE;
    writeTemp.startAddress = WRITE_START.toInt();
    writeTemp.endAddress = WRITE_END.toInt();
    readTemp.startAddress = READ_START.toInt();
    readTemp.endAddress = READ_END.toInt();
    // mbdata.writevalue = (uint16_t *)SEND_VALUE.toInt();
    // mbdata.readvalue = (uint16_t *)RECEIVE_VALUE.toInt();
    ModbusSetting();

}