#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include <header.h>

#include <FileSystem/FileSystem.h>
#include <Communication/MB.h>
#include <Sensor/SICK.h>
#include <OnlineManage/OnlineManage.h>
#include <OnlineManage/NPTTime.h>
#include <Sensor/SensorTask.h>

FFS filesystem;
OnlineManage online;
MODBUS_RTU modbusRTU;
MODBUS_TCP modbusTCP;
MODBUS_PARAMETER mbParam;

void setup()
{
  Serial.begin(115200);
  filesystem.Init();
  xTaskCreatePinnedToCore(TaskSICKSensor, "TaskSICKSensor", 5000, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(TaskOnlineManager, "TaskOnlineManager", 5000, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(TaskModbus, "TaskModbusRTU", 10000, NULL, 3, NULL, 1);
}

void loop()
{
}