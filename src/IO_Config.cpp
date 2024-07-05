#include <IO_Config.h>
#include <Arduino.h>

void IO_CONFIG::Init() {
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
}
