#include <header.h>
#include <OnlineManage/OnlineManage.h>
#include <Sensor/SICK.h>

// GLOBAL VARIABLES
String OD2000 = "{\"iolink\":{\"valid\":true,\"value\":[252,35,116,55,247,3]},\"iqValue\":true}";
String WTM10L = "{\"iolink\":{\"valid\":true,\"value\":[0,183,0,122]}}";
String WTM10L_False = "{\"iolink\":{\"valid\":true,\"value\":[127,255,0,66]}}";

String MPB10_1 = "{\"iolink\":{\"valid\":true,\"value\":[66,4,0,0,62,73,112,48,62,16,198,99,62,13,94,193,0,0,0,0]},\"iqValue\":false}";
String MPB10_2 = "{\"iolink\":{\"valid\":true,\"value\":[66,4,0,0,62,64,14,221,61,228,51,73,62,2,119,186,0,0,0,0]},\"iqValue\":false}";

void getOD2000Value(String JsonString)
{
    byte DataArr[6];
    JsonDocument doc;
    Serial.println(JsonString);
    deserializeJson(doc, JsonString);
    for (byte i = 0; i < 6; i++)
    {
        DataArr[i] = doc["iolink"]["value"][i];
    }
    JsonDocument sdoc;
    JsonString = "";
    sdoc["OD2000"]["value"][0] = sickSensor.getByte2IntValue(DataArr, 130, -6);
    serializeJson(sdoc, JsonString);
    Serial.println(JsonString);
    online.notifyClients(JsonString);
}
void getMPB10Value(String JsonString)
{
    byte DataArr[20];
    JsonDocument doc;
    Serial.println(JsonString);
    deserializeJson(doc, JsonString);
    for (byte i = 0; i < 20; i++)
    {
        DataArr[i] = doc["iolink"]["value"][i];
    }
    JsonDocument sdoc;
    JsonString = "";
    for (byte i = 0; i < 4; i++)
    {
        sdoc["MPB10"]["value"][i] = sickSensor.getByte2FloatValue(&DataArr[i * 4]);
    }
    serializeJson(sdoc, JsonString);
    Serial.println(JsonString);
    online.notifyClients(JsonString);
}
void getWTM10LValue(String JsonString)
{
    byte DataArr[4];
    JsonDocument doc;
    Serial.println(JsonString);
    deserializeJson(doc, JsonString);
    for (byte i = 0; i < 4; i++)
    {
        DataArr[i] = doc["iolink"]["value"][i];
    }
    if (DataArr[0] == 127 && DataArr[1] == 255)
    {
        return; // Error condition
    }
    JsonDocument sdoc;
    JsonString = "";
    sdoc["WTM10L"]["value"][0] = sickSensor.getByte2IntValue(DataArr, 0, 0, 2);
    serializeJson(sdoc, JsonString);
    Serial.println(JsonString);
    online.notifyClients(JsonString);
}

void TaskSICKSensor(void *pvParameter)
{
    float *gyroArr;
    for (;;)
    {
        getOD2000Value(OD2000);
        getMPB10Value(MPB10_1);
        getWTM10LValue(WTM10L);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}