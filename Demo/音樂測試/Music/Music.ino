/*
  音階測試
  預設的蜂鳴器正極接在D9腳位
*/
#include "pitches.h"

const byte notePin = 9;

int melody[] = {C4, D4, E4, F4, G4, A5, B5, C5};

int noteDurations[] = {2, 2, 2, 2, 2, 2, 2, 2};

void setup() {
}

void loop() {
  for (int i = 0; i < 8; i++) {
    int noteDuration = 1000 / noteDurations[i];
    tone(notePin, melody[i], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(notePin);
  }
  delay(1000);
}
