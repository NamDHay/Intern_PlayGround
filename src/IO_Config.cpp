#include <IO_Config.h>
#include <Arduino.h>

void IO_CONFIG::Init() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
}
