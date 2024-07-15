#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include <IO_Config.h>
#include <File_System.h>
#include <MB.h>
#include <OnlineManage.h>

IO_CONFIG io_config;
FFS filesystem;
OnlineManage online;
MODBUS_RTU modbus;

void setup() {
  Serial.begin(115200);
  io_config.Init();
  filesystem.Init();
  online.Init();
  xTaskCreatePinnedToCore(TaskOnlineManager,"TaskOnlineManager",5000,NULL,2,NULL,1);
  xTaskCreatePinnedToCore(TaskModbus,"TaskModbusRTU", 5000, NULL,1,NULL,1);
}

void loop(){
  //Nothing to do here
}