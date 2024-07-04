#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include <IO_Config.h>
#include <File_System.h>
#include <OnlineManage.h>

IO_CONFIG io_config;
FFS filesystem;
OnlineManage online;

void setup() {
  io_config.Init();
  filesystem.Init();
  online.Init();

  xTaskCreatePinnedToCore(TaskOnlineManager,"TaskOnlineManager",10000,NULL,2,NULL,1);
}

void loop(){
}