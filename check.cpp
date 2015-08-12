#include <iostream>
#include <stdio.h> // getchar
#include <stdint.h>

const char * associations[] = {
	"L1 ",
	"L2 ",
	"L3 ",
	"L4 ",
	"L5 ",
	"L6 ",
	"L7 ",
	"L8 ",
	"L9 ",
	"L10",
	"L11",
	"L12",
};

// Midi notes
enum MidiNote {
  MidiNote_C = 'C',  MidiNote_Cs = 'c',
  MidiNote_D = 'D',  MidiNote_Ds = 'd',
  MidiNote_E = 'E',
  MidiNote_F = 'F',  MidiNote_Fs = 'f',
  MidiNote_G = 'G',  MidiNote_Gs = 'g',
  MidiNote_A = 'A',  MidiNote_As = 'a',
  MidiNote_B = 'B'
};

// Midi octaves
enum MidiOctave {
  MidiOctave_m1 = 0,
  MidiOctave_0 = 12,
  MidiOctave_1 = 24,
  MidiOctave_2 = 36,
  MidiOctave_3 = 48,
  MidiOctave_4 = 60,
  MidiOctave_5 = 72,
  MidiOctave_6 = 84,
  MidiOctave_7 = 96,
  MidiOctave_8 = 108,
  MidiOctave_9 = 120
};

// Midi channels
enum MidiChannel {
  MidiChannel_1 = 1,  MidiChannel_2,   MidiChannel_3,  MidiChannel_4,
  MidiChannel_5,  MidiChannel_6,   MidiChannel_7,  MidiChannel_8,
  MidiChannel_9,  MidiChannel_10,  MidiChannel_11, MidiChannel_12,
  MidiChannel_13, MidiChannel_14,  MidiChannel_15, MidiChannel_16
};

#define NO_ARDUINO
#include "dhaum_arduino/dhaum_arduino.ino" // OK, thats ugly

using namespace std;

int main() {
	for (DhaumBinderIndex i = 0; i < binders_size; ++i) {
		DhaumBinderConf comb = get_binderconf_nr(i);
		cout << "::";

		DhaumBits bits = comb.bits;
		for (int j = 0; j < 12; ++j) {
			if (bits & 1) {
				cout << " " << associations[j];
			}
			bits >>= 1;
		}
		cout << endl
		     << comb.midi.getChannel() << "-"
		     << (comb.midi.getOctave() / 12 - 1)
		     << (char)(comb.midi.getNote())
		     << endl;

		getchar();
	}
	return 0;
}
