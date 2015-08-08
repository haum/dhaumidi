#ifndef DHAUM_H
#define DHAUM_H

#include <Arduino.h>

typedef uint32_t DhaumBits;
typedef uint16_t DhaumBinderIndex;

struct DhaumObject {
  DhaumBits bits;
  DhaumObject(uint8_t pad_nr) : bits(1 << (pad_nr-1)) {}
  DhaumObject operator | (const DhaumObject &other) {
    DhaumObject res(other);
    res.bits |= bits;
    return res;
  }
};

struct DhaumMidi {
  uint8_t packed_data[4];
  DhaumMidi(enum MidiNote note, enum MidiOctave octave = MidiOctave_3, MidiChannel channel = MidiChannel_1, uint8_t velocity = 64) {
    packed_data[0] = note;
    packed_data[1] = octave / 12;
    packed_data[2] = channel;
    packed_data[3] = velocity;
  }
  enum MidiNote getNote() { return (enum MidiNote)(packed_data[0]); }
  enum MidiOctave getOctave() { return (enum MidiOctave)(12 * packed_data[1]); }
  enum MidiChannel getChannel() { return (enum MidiChannel)(packed_data[2]); }
  uint8_t getVelocity() { return packed_data[3]; };
};

typedef enum {
  UNTOUCHED,
  TOUCHED,
} touchstatus_e;

struct DhaumBinder {
  DhaumBits bits;
  DhaumBits mask;
  DhaumMidi midi;
  int8_t debounce_t_tf; // debounce (6), touched (1), touched filtered (1) packing

  DhaumBinder(DhaumObject obj, DhaumMidi param_midi, uint32_t param_mask = 0xffffffff) : bits(obj.bits), mask(param_mask), midi(param_midi) {}
  touchstatus_e touched() { return ((debounce_t_tf & 1) == 1) ? TOUCHED : UNTOUCHED; }
  touchstatus_e touchedFiltered() { return ((debounce_t_tf & 2) == 2) ? TOUCHED : UNTOUCHED; }
  uint8_t getDebounce() { return (debounce_t_tf >> 2); }
  void setTouched(touchstatus_e touched) { debounce_t_tf = (debounce_t_tf & ~1) | (touched); }
  void setTouchedFiltered(touchstatus_e touched) { debounce_t_tf = (debounce_t_tf & ~2) | (touched << 1); }
  void setDebounce(int8_t debounce) { debounce_t_tf = (debounce_t_tf & 3) | (debounce << 2); }
};

extern DhaumBinder * binders;
extern DhaumBinderIndex binders_size;

#endif
