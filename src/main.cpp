#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include <File_System.h>
#include <MB.h>
#include <SICK.h>
#include <OnlineManage.h>
#include <NPTTime.h>

FFS filesystem;
OnlineManage online;
MODBUS_RTU modbusRTU;
MODBUS_TCP modbusTCP;
MODBUS_PARAMETER mbParam;
SICK_SENSOR sickSensor;

void setup()
{
  Serial.begin(115200);
  filesystem.Init();
  online.Init();
  npt.setup();
  xTaskCreatePinnedToCore(TaskSICKSensor, "TaskSICKSensor", 5000, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(TaskOnlineManager, "TaskOnlineManager", 5000, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(TaskModbus, "TaskModbusRTU", 10000, NULL, 3, NULL, 1);
}

void loop()
{
  // Nothing to do here
  //  npt.printLocalTime();
  //  delay(10000);
}