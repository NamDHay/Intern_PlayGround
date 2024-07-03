#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <main.h>
#include <CaptivePortal.h>
#include <Wifi_Config.h>
#include <File_System.h>

#define LED 25

class CONFIG {
private:

public:
    void Init();
    void Loop();
};

extern CONFIG config;

#endif //__CONFIG_H__
