byte count = 10;
void fileproccess_example(){
  writefile("/data.txt", "hello file data.txt\n");
  Serial.println(readfile("/data.txt"));
  writeafile("/data.txt", " 123\n");
  Serial.println(readfile("/data.txt"));
  for (byte i = 0; i < count; i++)
  {
    String noidung = String(i) + " | " ;
    writeafile("/data.txt", noidung);
  }
  
  Serial.println(readfile("/data.txt"));
}

String dataRead = "";
void loadSetting(){
  //load setting
  JsonDocument doc;//
  dataRead = readfile("/setting.json");
  deserializeJson(doc, dataRead);
  String SSID = doc["SSID"].as<String>();
  String PASS = doc["PASS"].as<String>();
  LOGLN("SSID: " + SSID);
  LOGLN("PASS: "+PASS); 
  settings.ssid = SSID;
  settings.pass = PASS;
}

String setting = "";
void writeSetting(){
  //Setting 
  JsonDocument writeDoc;
  writeDoc["SSID"] = settings.ssid;
  writeDoc["PASS"] = settings.pass;
  serializeJson(writeDoc, setting);
  LOGLN("JSON: "+ setting);
  writefile("/setting.json", setting);
}