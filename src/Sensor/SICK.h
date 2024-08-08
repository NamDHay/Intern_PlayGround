#ifndef __SICK_H__
#define __SICK_H__
#include <header.h>
#ifdef CONFIG_USE_SICK_SENSOR
#define CONFIG_USE_SICK_SENSOR
#include <Arduino.h>
class SICK_SENSOR
{
public:
    float getByte2IntValue(byte *arr, uint16_t zeroPos = 0, int8_t scale = 0, uint8_t length = 4);
    float getByte2FloatValue(byte *arr, uint16_t zeroPos = 0, int8_t scale = 0, uint8_t length = 4);
    float magnitudeCal(float x, float y, float z);

    float *getOD2000Value(String JsonString);
    float *getMPB10Value(String JsonString);
    uint16_t *getWTM10LValue(String JsonString);
    uint16_t *getCSSValue(String JsonString);
    float *getPBSValue(String JsonString);
};
extern SICK_SENSOR sickSensor;
#endif

#endif //__SICK_H__