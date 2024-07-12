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
    writeDoc["staip"] = settings.staticip;   
    writeDoc["waddress"] = settings.waddress;
    writeDoc["wsubnet"] = settings.wsubnet;
    writeDoc["wgetway"] = settings.wgetway ;
    writeDoc["wmode"] = settings.wifimode;

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
    String staip = doc["staip"].as<String>(); 
    String waddress = doc["waddress"].as<String>(); 
    String wsubnet = doc["wsubnet"].as<String>(); 
    String wgetway = doc["wgetway"].as<String>(); 
    String wmode = doc["wmode"].as<String>(); 

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
    LOGLN("staip: "+ staip); 
    LOGLN("wsubnet: "+ wsubnet); 
    LOGLN("waddress: "+ waddress); 
    LOGLN("wgetway: "+ wgetway); 
    LOGLN("wmode: "+ wmode);

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
    settings.staticip = staip;
    settings.waddress = waddress;
    settings.wsubnet = wsubnet;
    settings.wgetway = wgetway;
    settings.wifimode = wmode;

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