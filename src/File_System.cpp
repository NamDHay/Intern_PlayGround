#include <File_System.h>
#include <Arduino.h>

void FFS::Init(){
    if(!SPIFFS.begin(true)){
        Serial.println("An error occurred while mounting SPIFFS");
        return;
    }
}

void FFS::writefile(String filename, String content, uint8_t mode) {
    File file = SPIFFS.open(filename, (mode == 1 ? "a" : "w"));
    // Serial.println("File Write: ");
    file.print(content);
    Serial.println(content);
    file.close();
}

String FFS::readfile(String filename) {
    String content = "";
    File file = SPIFFS.open(filename,"r");
    if(!file){
        content = "Failed to open file for reading";
    }else{
        // Serial.println("File Open: ");
        while(file.available()){
            content += (char)file.read();
        }
        file.close();
    }
    return content;
}

bool FFS::deletefile(String filename) {
    return SPIFFS.remove(filename);
}