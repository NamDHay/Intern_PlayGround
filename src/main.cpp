#include <main.h>

void setup() {
  Serial.begin(115200);
  SPIFFS_Init();
  loadSetting();
  WifiConnect();
  WebInit();
  TaskHandle_t *taskHandlePortal = GetPortalTaskHandle();
  xTaskCreatePinnedToCore(TaskPortal, "TaskPortal", 10000, NULL, 1, taskHandlePortal, 1);
  xTaskCreatePinnedToCore(TaskOnlineManager,"TaskOnlineManager",10000,NULL,2,NULL,1);
  xTaskCreatePinnedToCore(TaskSocketHandle,"TaskOnlineManager",10000,NULL,1,NULL,1);
}

void loop(){
  //Nothing Here
}