#include <Sensor/SICK.h>
#ifdef CONFIG_USE_SICK_SENSOR
#include <Arduino.h>
#include <math.h>
#include <ArduinoJson.h>

#define CHECKBIT(Byte, BitPos) ((((Byte) & (1 << BitPos)) == (1 << BitPos)) ? 1 : 0)

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

float* SICK_SENSOR::getOD2000Value(String JsonString)
{
    byte DataArr[6];
    static float reArr[3];
    JsonDocument doc;
    deserializeJson(doc, JsonString);
    for (byte i = 0; i < 6; i++)
    {
        DataArr[i] = doc["iolink"]["value"][i];
    }

    reArr[0] = sickSensor.getByte2IntValue(DataArr, 130, -6);
    
    reArr[1] = CHECKBIT(DataArr[5], 0);
    reArr[2] = CHECKBIT(DataArr[5], 1);
    return reArr;
}
float* SICK_SENSOR::getMPB10Value(String JsonString)
{
    byte DataArr[20];
    static float Value[4];
    JsonDocument rdoc;
    deserializeJson(rdoc, JsonString);
    for (byte i = 0; i < 20; i++)
    {
        DataArr[i] = rdoc["iolink"]["value"][i];
    }
    for (byte i = 0; i < 4; i++)
    {
        Value[i] = sickSensor.getByte2FloatValue(&DataArr[i * 4]);
    }
    return Value;
}
uint16_t* SICK_SENSOR::getWTM10LValue(String JsonString)
{
    byte DataArr[4];
    static uint16_t reArr[3];
    JsonDocument doc;
    deserializeJson(doc, JsonString);
    for (byte i = 0; i < 4; i++)
    {
        DataArr[i] = doc["iolink"]["value"][i];
    }
    reArr[0] = sickSensor.getByte2IntValue(DataArr, 0, 0, 2);
    reArr[1] = CHECKBIT(DataArr[3], 0);
    reArr[2] = CHECKBIT(DataArr[3], 1);
    return reArr;
}
uint16_t* SICK_SENSOR::getCSSValue(String JsonString)
{
    byte DataArr[12];
    static uint16_t reArr[4];
    JsonDocument doc;
    deserializeJson(doc, JsonString);
    for (byte i = 0; i < 12; i++)
    {
        DataArr[i] = doc["iolink"]["value"][i];
    }
    reArr[0] = sickSensor.getByte2IntValue(&DataArr[0], 0, 0, 2);
    reArr[1] = sickSensor.getByte2IntValue(&DataArr[2], 0, 0, 2);
    reArr[2] = sickSensor.getByte2IntValue(&DataArr[4], 0, 0, 2);
    reArr[3] = sickSensor.getByte2IntValue(&DataArr[6], 0, 0, 2);
    reArr[4] = CHECKBIT(DataArr[11], 0);
    reArr[5] = CHECKBIT(DataArr[11], 1);
    return reArr;
}
float* SICK_SENSOR::getPBSValue(String JsonString)
{
    byte DataArr[5];
    static float reArr[3];
    JsonDocument doc;
    deserializeJson(doc, JsonString);
    for (byte i = 0; i < 5; i++)
    {
        DataArr[i] = doc["iolink"]["value"][i];
    }
    reArr[0] = sickSensor.getByte2FloatValue(DataArr);
    reArr[1] = CHECKBIT(DataArr[4], 0);
    reArr[2] = CHECKBIT(DataArr[4], 1);
    return reArr;
}
#endif
