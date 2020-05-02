/*
   按鈕觸動開關切換繼電器測試
   實驗一：按一下紅色按鈕，繼電器會切換開與關，但是如果一直按著紅色按鈕，繼電器會一直切換開與關。
   實驗二：優化後的按鈕開關，不會因為長按而循環開關狀態，為比較實用的觸動開關。
   使用時，只要把"/*/ (二)"的星號搬到"// (一)"兩斜槓中間就可以切換程式碼。
*/
byte btn = 12;  //按鈕開關PIN腳
byte rel = 10;  //繼電器開關PIN腳

void setup() {
  pinMode(btn, INPUT_PULLUP);
  pinMode(rel, OUTPUT);
}

void loop() {
  // (一)、簡易的觸動開關
  if (digitalRead(btn) == LOW) {
    delay(30);
    if (digitalRead(btn) == LOW) {
      digitalWrite(rel, ! digitalRead(rel));
      delay(1000);
    }
  }
  //*/

  /*/ (二)、優化後的觸動開關
  static bool bst;
  if (digitalRead(btn) == LOW) {
    delay(30);
    if (digitalRead(btn) == LOW && bst == HIGH) {
      bst = LOW;
      digitalWrite(rel, ! digitalRead(rel));
    }
  }
  if (digitalRead(btn) == HIGH) {
    delay(30);
    if (digitalRead(btn) == HIGH)
      bst = HIGH;
  }
  //*/
}
