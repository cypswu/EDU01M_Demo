/*
   EWCIoT EDU01M ESP01M教學開發板
   系統更新： 韌體更新為預載系統

   編譯版本： ESP8266 2.6.3
   created 2019.12.26 by lung <http://www.ewciot.com>
*/

#include <ESP8266httpUpdate.h>

const char* ssid     = "ssid";
const char* password = "password";
const char* url = "https://raw.githubusercontent.com/cypswu/EDU01M_Demo/master/Demo/EWC_EDU01M_V1_6.bin";
const char* tlsFinger = "ccaa484866460e91532c9c7c232ab1744d299d33";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected, updating");
  delay(1000);
  update();
}

void loop() {
}

void update() {
  t_httpUpdate_return ret = ESPhttpUpdate.update(url, "", tlsFinger);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("UPDATE_Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("NO_UPDATES");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("UPDATE_OK");
      break;
  }
}
