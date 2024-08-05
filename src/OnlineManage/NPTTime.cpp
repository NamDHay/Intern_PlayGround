#include <header.h>
#ifdef CONFIG_USE_NPT_TIME
#include <WiFi.h>
#include <OnlineManage/NPTTime.h>

NPT npt;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 7;

void NPT::setup(void)
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); 
}

void NPT::printLocalTime(void)
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  char timeHour[3], timeMin[3], timeSec[3];
  strftime(timeHour,3, "%H", &timeinfo);
  strftime(timeMin ,3, "%M", &timeinfo);
  strftime(timeSec ,3, "%S", &timeinfo);

  Serial.print("Realtime: "); 
  Serial.print(timeHour);  Serial.print(" : ");
  Serial.print(timeMin);  Serial.print(" : ");
  Serial.print(timeSec);  Serial.print("\r\n");
}
#endif