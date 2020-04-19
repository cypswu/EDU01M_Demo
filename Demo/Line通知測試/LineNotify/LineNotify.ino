/*
   LINE通知測試
*/

#include <TridentTD_LineNotify.h>  // https://github.com/TridentTD/TridentTD_LineNotify

#define SSID        "ssid"
#define PASSWORD    "password"
#define LINE_TOKEN  ""

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println(LINE.getVersion());

  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());

  LINE.setToken(LINE_TOKEN);

  LINE.notify("我的通知測試");

  // LINE內建貼圖 https://devdocs.line.me/files/sticker_list.pdf
  LINE.notifySticker(2, 45);       // 貼圖 PackageID 3 , StickerID 240

  Serial.println("Send notify");
}

void loop() {
  delay(1);
}
