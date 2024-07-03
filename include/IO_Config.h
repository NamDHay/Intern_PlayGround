#ifndef __IO_CONFIG_H__
#define __IO_CONFIG_H__

#include <Arduino.h>

#define LED 25

class IO_CONFIG {
public:
    void Init();
};

extern IO_CONFIG io_config;

#endif //__IO_CONFIG_H__
