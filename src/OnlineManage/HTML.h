#ifndef __HTML_H__
#define __HTML_H__

#include <Arduino.h>

#ifdef CONFIG_USE_WIFI_CONFIG_HTML
class Website
{
public:
  String Wifi_Config_HTML = "\
                            <!DOCTYPE html>\
                            <html>\
                            <body><CENTER>\
                            <h2>Cai dat Wifi</h2>\
                            <form action=\"/get\">\
                              <label for=\"fname\">ten wifi:</label><br>\
                              <input type=\"text\" id=\"ssid\" name=\"ssid\" placeholder=\"ten wifi\"><br>\
                              <label for=\"lname\">mat khau wifi:</label><br>\
                              <input type=\"password\" id=\"pass\" name=\"pass\" placeholder=\"pass wifi\"><br><br>\
                              <input type=\"submit\" value=\"Luu\">\
                            </form> </CENTER>\
                            </body>\
                            </html>\
";
};
extern Website mywebsite;
#endif // CONFIG_USE_WIFI_CONFIG_HTML

#endif //__HTML_H__
