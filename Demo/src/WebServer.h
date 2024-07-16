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

    String slaveID = doc["slaveID"].as<String>(); 
    String baud = doc["baud"].as<String>(); 
    String typedata = doc["typedata"].as<String>();
    String serial = doc["serial"].as<String>(); 
    String mbmaster = doc["mbmaster"].as<String>(); 
    String writeStart = doc["writeStart"].as<String>(); 
    String writeEnd = doc["writeEnd"].as<String>(); 
    String readStart = doc["readStart"].as<String>(); 
    String readEnd = doc["readEnd"].as<String>(); 

    LOGLN("SSID: " + SSID);
    LOGLN("PASS: " + PASS);
    LOGLN("staip: "+ staip); 
    LOGLN("wsubnet: "+ wsubnet); 
    LOGLN("waddress: "+ waddress); 
    LOGLN("wgetway: "+ wgetway); 
    LOGLN("wmode: "+ wmode);

    LOGLN("slaveID: "+ slaveID); 
    LOGLN("baud: "+ baud); 
    LOGLN("serial: "+ serial); 
    LOGLN("mbmaster: "+ mbmaster); 
    LOGLN("writeStart: "+ writeStart); 
    LOGLN("writeEnd: "+ writeEnd); 
    LOGLN("readStart: "+ readStart); 
    LOGLN("readEnd: "+ readEnd); 

    settings.ssid = SSID;
    settings.pass = PASS;
    settings.staticip = staip;
    settings.waddress = waddress;
    settings.wsubnet = wsubnet;
    settings.wgetway = wgetway;
    settings.wifimode = wmode;
    writeSetting();
}