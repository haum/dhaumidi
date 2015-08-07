#ifndef DHAUM_H
#define DHAUM_H

#include <Arduino.h>

#define MIDIOCTAVE_OFFSET 24 // so we get 1 octave below C3 and 2 above B3

typedef uint32_t DhaumBits;


struct DhaumMidi {
  enum MidiNote note;
  enum MidiOctave octave;
  enum MidiChannel channel;
  uint8_t velocity;

  DhaumMidi(enum MidiNote param_note, enum MidiOctave param_octave = MidiOctave_3, MidiChannel param_channel = MidiChannel_1, uint8_t param_velocity = 64) :
    note(param_note), octave(param_octave), channel(param_channel), velocity(param_velocity) {}
};

struct DhaumNotes {
  DhaumMidi* notes;
  uint8_t count = 0;
};

extern DhaumNotes extract_notes(DhaumBits snapshot);
#endif
