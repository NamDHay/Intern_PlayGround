#include <main.h>
#include <BoardParameter.h>
#include <Config.h>

void setup() {
  Serial.begin(115200);
  Config_Output();
  SPIFFS_Init();
  loadSetting();
  WifiConnect();
  WebInit();
  config.Init();
  
  TaskHandle_t *taskHandlePortal = GetPortalTaskHandle();
  xTaskCreatePinnedToCore(TaskPortal, "TaskPortal", 10000, NULL, 1, taskHandlePortal, 1);
  xTaskCreatePinnedToCore(TaskOnlineManager,"TaskOnlineManager",10000,NULL,2,NULL,1);
  xTaskCreatePinnedToCore(TaskSocketHandle,"TaskOnlineManager",10000,NULL,1,NULL,1);
}

void loop(){
  config.Loop();
}