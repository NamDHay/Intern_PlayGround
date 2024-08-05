#ifndef __SICK_H__
#define __SICK_H__

#include <Arduino.h>
#include <math.h>

class SICK_SENSOR
{
public:
    float getByte2IntValue(byte *arr, uint16_t zeroPos = 0, int8_t scale = 0, uint8_t length = 4);
    float getByte2FloatValue(byte *arr, uint16_t zeroPos = 0, int8_t scale = 0, uint8_t length = 4);

#ifdef MPB10_SICK
    float magnitudeCal(float x, float y, float z);
#endif // MPB10_SICK
};
void TaskSICKSensor(void *pvParameter);
extern SICK_SENSOR sickSensor;

#endif //__SICK_H__