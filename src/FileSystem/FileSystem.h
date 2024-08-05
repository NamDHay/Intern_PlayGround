#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#ifdef CONFIG_USE_FILE_SYSTEM
#include <SPIFFS.h>
class FFS
{
public:
    void Init();
    void writefile(String filename, String content, uint8_t mode);
    String readfile(String filename);
    bool deletefile(String filename);
    String ListFile();
};
extern FFS filesystem;
#endif //CONFIG_USE_FILE_SYSTEM
#endif //__FILE_SYSTEM_H__