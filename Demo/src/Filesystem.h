#include <Arduino.h>

String readfile(String filename){
  String content = "";
  File file = SPIFFS.open(filename,"r");
  if(!file){
    Serial.println("Failed to open file for reading");
    content = "Failed to open file for reading";
  }else{
  Serial.println("File Open: ");
  while(file.available()){
    content += (char)file.read();
  }
  file.close();}
  return content;
}

void writefile(String filename, String content){
  File file = SPIFFS.open(filename, "w");
  Serial.println("File Write: ");
  file.print(content);
  Serial.println(content);
  file.close();
}//writefile

void writeafile(String filename, String content){
  File file = SPIFFS.open(filename, "a");
  Serial.println("File Write a: ");
  file.print(content);
  Serial.println(content);
  file.close();
}//writeafile

void SPIFFS_Init(){
  if(!SPIFFS.begin(true)){
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }
}//SPIFFS_Init