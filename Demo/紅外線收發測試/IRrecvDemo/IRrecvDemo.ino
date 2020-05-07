/*
   紅外線接收測試
   此範例需使用其他紅外線遙控器測試接收訊號並顯示輸出
*/

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// 定義紅外線接收使用PIN腳
const uint16_t RecvPin = 14;

IRrecv irrecv(RecvPin);

decode_results results;  // 解碼紅外線數據

void setup() {
  Serial.begin(115200);
  Serial.println();
  irrecv.enableIRIn();   // 開始啟用紅外線接收
}

void loop() {
  if (irrecv.decode(&results)) {
    serialPrintUint64(results.value, HEX);
    Serial.println();
    irrecv.resume();     // 準備接收下一個紅外線訊號
  }
  delay(100);
}
