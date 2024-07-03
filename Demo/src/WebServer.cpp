#include <Arduino.h>
#include <WebServer.h>
FileSystem FSFunc;

void Function::writeSetting(){
    //Setting 
    JsonDocument writeDoc;
    writeDoc["SSID"] = settings.ssid;
    writeDoc["PASS"] = settings.pass;
    serializeJson(writeDoc, setting);
    LOGLN("JSON: "+ setting);
    FSFunc.writefile("/setting.json", setting);
}
void Function::loadSetting(){
    //load setting
    JsonDocument doc;//
    dataRead = FSFunc.readfile("/setting.json");
    deserializeJson(doc, dataRead);
    String SSID = doc["SSID"].as<String>();
    String PASS = doc["PASS"].as<String>();
    LOGLN("SSID: " + SSID);
    LOGLN("PASS: "+ PASS); 
    settings.ssid = SSID;
    settings.pass = PASS;
    writeSetting();
}

