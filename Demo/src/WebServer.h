#ifndef WebServer_
#define WedServe_
#define LOG(x) {Serial.print(x);}
#define LOGLN(x) {Serial.println(x);}
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Filesystem.h>
class WebServer
{
private:

public:
    String dataRead = "";
    String setting = "";
    struct Settings
    {
        String ssid;
        String pass;
    }settings;
    void loadSetting();
    void writeSetting();


};
extern WebServer WEBSERVER;
#endif