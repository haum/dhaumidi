#include "dhaum.h"

#define MAX_BINDERS 102
DhaumBinderData bindersd[MAX_BINDERS];
DhaumBinderIndex binders_size = MAX_BINDERS;
DhaumBinderData * binders_data = bindersd;

// Sounds
static const MidiNote notes7[] = { MidiNote_C, MidiNote_D, MidiNote_E, MidiNote_F, MidiNote_G, MidiNote_A, MidiNote_B };
static const MidiNote notes12[] = { MidiNote_C, MidiNote_Cs, MidiNote_D, MidiNote_Ds, MidiNote_E, MidiNote_F, MidiNote_Fs, MidiNote_G, MidiNote_Gs, MidiNote_A, MidiNote_As, MidiNote_B };
static const MidiOctave octaves[] = { MidiOctave_2,  MidiOctave_3,  MidiOctave_4,  MidiOctave_5 };

DhaumBits gen_combi(DhaumBinderIndex nb) {
#define COMBI(x, n, j) if (n < j) return (((x << n) | ((x << n) >> 12)) & 0xFFF); n -= j;
  COMBI(0x249, nb, 3)
  COMBI(0x145, nb, 5)
  COMBI(0x111, nb, 4)
  COMBI(0x00b, nb, 12)
  COMBI(0x049, nb, 12)
  COMBI(0x091, nb, 12)
  COMBI(0x045, nb, 12)
  COMBI(0x021, nb, 6)
  COMBI(0x003, nb, 12)
  COMBI(0x005, nb, 12)
  COMBI(0x009, nb, 12)
#undef COMBI
  return 0;
}

DhaumBinderConf get_binderconf_nr(DhaumBinderIndex i) {
  DhaumBits combi = gen_combi(i);
  MidiChannel channel = MidiChannel_1;
  MidiNote note = MidiNote_C;
  MidiOctave octave = MidiOctave_2;

  if (i < 8) {
    note = notes7[i % 7];
    octave = octaves[(i < 7) + 1];
    channel = MidiChannel_4;
  } else if (i < 24) {
    DhaumBinderIndex j = i-8;
    note = notes12[j % 12];
    octave = octaves[j / 12 + 1];
    channel = MidiChannel_3;
  } else if (i < 60) {
    DhaumBinderIndex j = i-24;
    note = notes12[j % 12];
    octave = octaves[j / 12];
    channel = MidiChannel_2;
  } else {
    DhaumBinderIndex j = i-60;
    note = notes12[j % 12];
    octave = octaves[j / 12];
  }

  DhaumMidi midi(note, octave, channel);
  DhaumBinderConf conf(combi, midi);
  return conf;
}
