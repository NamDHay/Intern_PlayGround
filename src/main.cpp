#include <main.h>

void setup() {
  Serial.begin(115200);
  SPIFFS_Init();
  loadSetting();
  WifiConnect();
  WebInit();
      // Serial.println("ESP free heep: " + ESP.getFreeHeap());
  xTaskCreatePinnedToCore(TaskOnlineManager,"TaskOnlineManager",10000,NULL,2,NULL,1);
}

void loop(){
  //Nothing here
  static long taskStartTime = millis();
  if(millis() - taskStartTime > 10000){
    Serial.println("ESP free Heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.println("ESP free RAM: ");
    Serial.println(ESP.getFreePsram());
    taskStartTime = millis();
  }
}