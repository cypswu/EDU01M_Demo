/*
   按鈕觸動開關切換繼電器測試
*/
byte btn = 12;
byte rel = 10;

void setup() {
  pinMode(btn, INPUT_PULLUP);
  pinMode(rel, OUTPUT);
}

void loop() {
  // 簡易的觸動開關
  if (digitalRead(btn) == LOW) {
    delay(30);
    if (digitalRead(btn) == LOW) {
      digitalWrite(rel, ! digitalRead(rel));
      delay(1000);
    }
  }
  //*/

  /*/ 優化後的觸動開關
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
