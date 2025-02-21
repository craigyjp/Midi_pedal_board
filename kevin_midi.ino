#include <MIDIUSB.h>

// set for 0-15 for channel 1-16
#define MIDI_CHANNEL 0

const int notePins[13] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 16, 14, 15, 18};
const int baseNote = 60; // C4
int octaveShift = 0;
bool keyState[13] = {false};
const char* noteNames[13] = {"C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4", "C5"};
const int shift1Button = 20;
const int shift2Button = 21;
bool shift1State = false;
bool shift2State = false;
bool lastShift1 = true;
bool lastShift2 = true;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 13; i++) {
    pinMode(notePins[i], INPUT_PULLUP);
  }
  pinMode(shift1Button, INPUT_PULLUP);
  pinMode(shift2Button, INPUT_PULLUP);
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}

void loop() {
  bool currentShift1 = digitalRead(shift1Button);
  bool currentShift2 = digitalRead(shift2Button);

  if (currentShift1 == LOW && currentShift2 == LOW) {
    octaveShift = (octaveShift == -3) ? 0 : -3;
    delay(300);
  } else {
    if (currentShift1 == LOW && lastShift1 == HIGH) {
      shift1State = !shift1State;
      delay(200);
    }
    lastShift1 = currentShift1;

    if (currentShift2 == LOW && lastShift2 == HIGH) {
      shift2State = !shift2State;
      delay(200);
    }
    lastShift2 = currentShift2;

    if (shift1State && shift2State) {
      octaveShift = -3;
    } else if (shift2State) {
      octaveShift = -2;
    } else if (shift1State) {
      octaveShift = -1;
    } else {
      octaveShift = 0;
    }
  }

  for (int i = 0; i < 13; i++) {
    bool pressed = digitalRead(notePins[i]) == LOW;
    if (pressed != keyState[i]) {
      keyState[i] = pressed;
      int note = baseNote + (octaveShift * 12) + i;
      if (pressed) {
        noteOn(MIDI_CHANNEL, note, 127);
        Serial.print("Note On: ");
      } else {
        noteOff(MIDI_CHANNEL, note, 0);
        Serial.print("Note Off: ");
      }
      Serial.print(noteNames[i]);
      Serial.print(" (MIDI Note: ");
      Serial.print(note);
      Serial.println(")");
    }
  }
  delay(5);
}
