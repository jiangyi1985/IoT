//+ --> Arduino Digital 3

#include "pitches.h"

// notes in the song 'Mukkathe Penne'
int melody[] = {
  NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
  NOTE_G4, NOTE_C5, NOTE_AS4, NOTE_A4,
  NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
  NOTE_C4, NOTE_D4, 0,

  NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_A4,
  NOTE_G4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4,  //29               //8
  NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_FS4, NOTE_DS4, NOTE_D4,
  NOTE_C4, NOTE_D4, 0,

  NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
  NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5,
  NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_FS4, NOTE_D5, NOTE_C5,
  NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_AS4,  //58

  NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_DS5, NOTE_D5,
  NOTE_C5, NOTE_D5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_AS4, NOTE_A4, NOTE_C5, NOTE_G4,
  NOTE_A4, 0, NOTE_AS4, NOTE_A4, 0, NOTE_G4,
  NOTE_G4, NOTE_A4, NOTE_G4, NOTE_FS4, 0,

  NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
  NOTE_C4, NOTE_D4, 0,
  NOTE_C4, NOTE_D4, NOTE_G4, NOTE_FS4, NOTE_DS4,
  NOTE_C4, NOTE_D4, END

};

// note durations: 8 = quarter note, 4 = 8th note, etc.
int noteDurations[] = {
  //duration of the notes
  8,
  4,
  8,
  4,
  4,
  4,
  4,
  12,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  16,
  4,

  8,
  4,
  8,
  4,
  4,
  2,
  1,
  1,
  2,
  1,
  1,
  12,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  16,
  4,

  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  12,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  12,

  4,
  4,
  4,
  4,
  4,
  4,
  2,
  1,
  1,
  2,
  1,
  1,
  4,
  8,
  4,
  2,
  6,
  4,
  2,
  6,
  4,
  2,
  1,
  1,
  16,
  4,

  4,
  8,
  4,
  4,
  4,
  4,
  16,
  4,
  4,
  8,
  4,
  4,
  4,
  4,
  20,
};

int speed = 90;  //higher value, slower notes
const int PIN_TONE = 16;

void setup() {

//why the reset button not working on nodemcu for this program???
//for aithinker board, hold down left button and press right button

  Serial.begin(115200);
  for (int thisNote = 0; melody[thisNote] != -1; thisNote++) {

    int noteDuration = speed * noteDurations[thisNote];
    tone(PIN_TONE, melody[thisNote], noteDuration * .95);
    Serial.println(melody[thisNote]);

    delay(noteDuration);

    noTone(PIN_TONE);
  }
}

void loop() {
  // no need to repeat the melody.
}
