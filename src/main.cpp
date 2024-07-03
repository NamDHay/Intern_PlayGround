#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <IO_Config.h>
#include <File_System.h>
#include <OnlineManage.h>

void setup() {
  io_config.Init();
  filesystem.Init();
  online.Init();

  // TaskHandle_t *taskHandlePortal = GetPortalTaskHandle();
  // xTaskCreatePinnedToCore(TaskPortal, "TaskPortal", 10000, NULL, 1, taskHandlePortal, 1);
  xTaskCreatePinnedToCore(TaskOnlineManager,"TaskOnlineManager",10000,NULL,2,NULL,1);
  xTaskCreatePinnedToCore(TaskSocketHandle,"TaskOnlineManager",10000,NULL,1,NULL,1);
}

void loop(){

}