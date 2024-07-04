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
    writeDoc[" WIFI_MODE"] = settings.wifimode;
    writeDoc[" CHANEL"] =  settings.chanel;
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
    LOGLN("SSID: " + SSID);
    LOGLN("STATIC_IP: "+ STATIC_IP); 
    LOGLN("WEB_SUBNET: "+ WEB_SUBNET); 
    LOGLN("WEB_ADDRESS: "+ WEB_ADDRESS); 
    LOGLN("WEB_GETWAY: "+ WEB_GETWAY); 
    LOGLN("WIFI_MODE: "+ WIFI_MODE); 
    LOGLN("CHANEL: "+ CHANEL); 
    settings.ssid = SSID;
    settings.pass = PASS;
    settings.staticip = STATIC_IP;
    settings.waddress = WEB_ADDRESS;
    settings.wsubnet = WEB_SUBNET;
    settings.wgetway = WEB_GETWAY;
    settings.wifimode = WIFI_MODE;
    settings.chanel = CHANEL;
    writeSetting();
}