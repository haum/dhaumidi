#ifndef DHAUM_H
#define DHAUM_H

#ifndef NO_ARDUINO
#include <Arduino.h>
#endif

typedef uint32_t DhaumBits;
typedef uint16_t DhaumBinderIndex;

struct DhaumObject {
  DhaumBits bits;
  explicit DhaumObject(uint8_t pad_nr) : bits(1 << (pad_nr-1)) {}
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
  DhaumMidi midi;
  int8_t debounce_t_tf; // debounce (5), touched filtered at least once since start (1), currently touched (1), currently touched filtered (1) packing

  DhaumBinder() : bits(-1), midi(DhaumMidi(MidiNote_C)) {}
  DhaumBinder(DhaumBits param_bits, DhaumMidi param_midi) : bits(param_bits), midi(param_midi) {}
  DhaumBinder(DhaumObject obj, DhaumMidi param_midi) : bits(obj.bits), midi(param_midi) {}
  touchstatus_e touched() { return ((debounce_t_tf & 1) == 1) ? TOUCHED : UNTOUCHED; }
  touchstatus_e touchedFiltered() { return ((debounce_t_tf & 2) == 2) ? TOUCHED : UNTOUCHED; }
  touchstatus_e touchedOnce() { return ((debounce_t_tf & 4) == 4) ? TOUCHED : UNTOUCHED; }
  uint8_t getDebounce() { return (debounce_t_tf >> 3); }
  void setTouched(touchstatus_e touched) { debounce_t_tf = (debounce_t_tf & ~1) | (touched); }
  void setTouchedFiltered(touchstatus_e touched) { debounce_t_tf = (debounce_t_tf & ~2) | (touched << 1); }
  void setTouchedOnce(touchstatus_e touched) { debounce_t_tf = (debounce_t_tf & ~4) | (touched << 2); }
  void setDebounce(int8_t debounce) { debounce_t_tf = (debounce_t_tf & 7) | (debounce << 3); }
};

extern DhaumBinder * binders;
extern DhaumBinderIndex binders_size;
void init_binders();

#endif
