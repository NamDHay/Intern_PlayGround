#ifndef __MAIN_H__
#define __MAIN_H__

#include <Wifi_Config.h>
#include <File_System.h>

String header;
String HTML = "\
<!DOCTYPE html>\
<html>\
<body><CENTER>\
<h2>Cai dat Wifi</h2>\
<form action=\"/get\">\
  <label for=\"fname\">ten wifi:</label><br>\
  <input type=\"text\" id=\"ssid\" name=\"ssid\" placeholder=\"ten wifi\"><br>\
  <label for=\"lname\">mat khau wifi:</label><br>\
  <input type=\"text\" id=\"pass\" name=\"pass\" placeholder=\"pass wifi\"><br><br>\
  <input type=\"submit\" value=\"Luu\">\
</form> </CENTER>\
</body>\
</html>\
";

#endif