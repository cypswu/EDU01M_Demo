/*
   按鈕開關與蜂鳴器測試
   按下紅色按鈕，蜂鳴器會發出聲音。
*/
byte btn = 12;  //開關按鈕PIN腳
byte bep = 9;   //蜂鳴器PIN腳

void setup() {
  pinMode(btn, INPUT_PULLUP);
  pinMode(bep, OUTPUT);
}

void loop() {
  if (digitalRead(btn) == LOW) {
    digitalWrite(bep, HIGH);
  } else {
    digitalWrite(bep, LOW);
  }
}
