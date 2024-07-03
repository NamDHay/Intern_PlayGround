#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#include <SPIFFS.h>

void SPIFFS_Init();
void writefile(String filename, String content, uint8_t mode);
String readfile(String filename);

#endif //__FILE_SYSTEM_H__