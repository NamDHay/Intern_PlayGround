#ifndef FileSystem_
#define FileSystem_
#include <Arduino.h>
#include <SPIFFS.h>
class FileSystem{
  public:
    String readfile(String filename);
    void writefile(String filename, String content);
    void writeafile(String filename, String content);
    void SPIFFS_Init();
  private:

};
extern FileSystem FILESYSTEM;
#endif