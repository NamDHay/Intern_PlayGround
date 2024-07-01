#include <main.h>

void fileproccess_example(){
  writefile("/data.txt", "hello file data.txt\n",0);
  Serial.println(readfile("/data.txt"));
  writefile("/data.txt", " 123\n",1);
  Serial.println(readfile("/data.txt"));
  for (byte i = 0; i < 10; i++)
  {
    String content = String(i) + " | " ;
    writefile("/data.txt", content,1);
  }
  Serial.println(readfile("/data.txt"));
}

void setup() {
  Serial.begin(115200);
  WifiConnect();
  SPIFFS_Init();
  fileproccess_example();
}

void loop() {
  //Nothing Here
}