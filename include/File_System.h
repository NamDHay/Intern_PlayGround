#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include <SPIFFS.h>

class FFS
{
public:
    void Init();
    void writefile(String filename, String content, uint8_t mode);
    String readfile(String filename);
    bool deletefile(String filename);
    void ListFile();
};

extern FFS filesystem;

#endif //__FILE_SYSTEM_H__