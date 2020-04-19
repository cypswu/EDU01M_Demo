/*
   按鈕開關與蜂鳴器測試
*/
byte btn = 12;
byte bep = 9;

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
