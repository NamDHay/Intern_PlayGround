#include <header.h>
#ifdef CONFIG_USE_SICK_SENSOR
#include <Arduino.h>
#include <math.h>
#include <Sensor/SICK.h>
SICK_SENSOR sickSensor;
float SICK_SENSOR::getByte2IntValue(byte *arr, uint16_t zeroPos, int8_t scale, uint8_t length)
{
    int Measurement_value = 0;
    for (byte i = 0; i < length; i++)
    {
        Measurement_value |= (((uint32_t)arr[i]) << (8 * (length - i - 1)));
    }
    float relativeData = Measurement_value * pow(10, scale);
    float absoluteData = relativeData + zeroPos;
    return absoluteData;
}
float SICK_SENSOR::getByte2FloatValue(byte *arr, uint16_t zeroPos, int8_t scale, uint8_t length)
{
    union type_t
    {
        float f;
        uint32_t u;
    } Measurement_value;
    Measurement_value.u = 0;
    for (byte i = 0; i < length; i++)
    {
        Measurement_value.u |= (((uint32_t)arr[i]) << (8 * (length - i - 1)));
    }
    float relativeData = Measurement_value.f * pow(10, scale);
    float absoluteData = relativeData + zeroPos;
    return absoluteData;
}

float SICK_SENSOR::magnitudeCal(float x, float y, float z)
{
    return sqrt(x * x + y * y + z * z);
}
#endif
