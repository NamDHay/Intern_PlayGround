#include <Config.h>
#include <Arduino.h>

void CONFIG::Init() {
    pinMode(LED, OUTPUT);
    digitalWrite(LED, 1);
    delay(1000);
    digitalWrite(LED, 0);
    delay(1000);
}

void CONFIG::Loop() {
    static long LED_starttime = millis();
    if(millis() - LED_starttime > 1000) {
        LED_starttime = millis();
        digitalWrite(LED, !digitalRead(LED));
    }
}