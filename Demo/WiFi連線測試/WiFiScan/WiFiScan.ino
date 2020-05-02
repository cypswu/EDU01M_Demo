/*
    掃描WiFi基地台
	RSSI是接收的訊號強度，顯示為負值，數值越大訊號越強(-30訊號強度 > -80訊號強度)，顯示有"*"表示為加密連線。
*/
#include "ESP8266WiFi.h"

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  Serial.println("開始掃描...");

  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("沒有找到WiFi基地台：");
  } else {
    Serial.print(n);
    Serial.println("WiFi基地台列表：");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");

  delay(10000);
}
9