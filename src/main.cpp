#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include <IO_Config.h>
#include <File_System.h>
#include <MB.h>
#include <OnlineManage.h>

void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(TaskOnlineManager,"TaskOnlineManager",1024,NULL,2,NULL,1);
  xTaskCreatePinnedToCore(TaskModbus,"TaskModbusRTU", 512, NULL,1,NULL,1);
}

void loop(){
  //Nothing to do here
}