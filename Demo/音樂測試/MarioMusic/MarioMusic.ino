/*
  瑪莉歐音樂
  預設的蜂鳴器正極接在D9腳位
*/
#include "pitches.h"

const byte notePin = 9;

void setup() {
}

void loop() {
  for (int thisNote = 0; thisNote < 98; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(notePin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(notePin);
  }
}
