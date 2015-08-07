#include "dhaum.h"

#define PADSNB (sizeof(L)/sizeof(*L))

void print_hex(const char * const str, DhaumBits hex, bool predicat) {
  if (!predicat) return;
  Serial.print(str);
  Serial.println(hex, HEX);
}

DhaumBits get_pads_snapshot() {
  DhaumBits snapshot = 0, curbit = 1;
  for (int i = 1; i < PADSNB; ++i) {
    if (!digitalRead(L[i]))
      snapshot |= curbit;
    curbit <<= 1;
  }
  return snapshot;
}

void setup() {
  // Reset pads
  for (int i = 0; i < PADSNB; ++i) {
    pinMode(L[i], INPUT);
    digitalWrite(L[i], LOW);
  }

  // Services begin
  Serial.begin(115200);
}

void loop () {
  static DhaumNotes notes;

  // read pads and get a global snapshot
  DhaumBits global_snapshot = 0;
  for (int i ; i < PADSNB ; i++) {
    pinMode(L[i], OUTPUT);
    global_snapshot ^= get_pads_snapshot();
    digitalWrite(L[i], HIGH);
    pinMode(L[i], INPUT);
    digitalWrite(L[i], LOW);
  }

  // TODO : debounce ?
  // it may not be necessary since we xor
  DhaumNotes new_notes = extract_notes(global_snapshot);

  // deactivate previously played notes
  // TODO : to not deactivate the currently playing notes
  //        may be consider comparing the snapshots instead of structs
  if (notes.count > 0) {
    for (int i ; i < notes.count ; i++) {
      MIDIUSB.note(0,  notes.notes[i].note, notes.notes[i].octave, notes.notes[i].channel, notes.notes[i].velocity);
    }
  }

  free(notes.notes); // ensure to free it, it's dynamic
  notes = new_notes;

  // play the new ones
  if (notes.count > 0) {
    for (int i ; i < notes.count ; i++) {
      MIDIUSB.note(1,  notes.notes[i].note, notes.notes[i].octave, notes.notes[i].channel, notes.notes[i].velocity);
    }
  }

}
