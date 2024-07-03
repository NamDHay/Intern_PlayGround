#include <Config.h>
#include <Arduino.h>

void CONFIG::Init() {
    Serial.begin(115200);
    digitalWrite(LED, 0);
}
