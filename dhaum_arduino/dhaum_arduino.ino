#include "dhaum.h"

DhaumNotes extract_notes(DhaumBits snapshot) {

	uint8_t DhaumVelocity[4] = {64, 55, 40, 80};

	MidiChannel channel = MidiChannel(snapshot & 0x100);
	MidiOctave octave = (MidiOctave) (MIDIOCTAVE_OFFSET * (snapshot & (DhaumBits) 0x10 << 8));
	uint8_t velocity = DhaumVelocity[snapshot & (DhaumBits) 0xF << 12];
	// compute notes

	/*
	 * 8 bits of the snapshot have already been used
	 * 17 are left
	 * Let's compute 4 notes out of them
	 *
	 */

	DhaumNotes midinotes;
	int8_t notes[4];
	DhaumBits mask = (0xFF << 16);
	int8_t nb_notes = 0;
	for (int8_t i = 0; i < 4; i++) {
		DhaumBits snapshot_extracted =  snapshot & (mask << 4*i);
		if (snapshot_extracted == 0) {
			notes[i] = -1;
			continue;
		} else if ( snapshot_extracted > 12 ) {
			snapshot_extracted = snapshot_extracted - 12;
		}
		notes[i] = MidiNote(snapshot_extracted-1);
    midinotes.count++;
	}

	midinotes.notes = (DhaumMidi*) malloc(nb_notes*sizeof(DhaumMidi));
	int8_t j = 0;
	for (int8_t i = 0; i < 4; i++) {
		if (notes[i] > 0) {
			midinotes.notes[j] = DhaumMidi((MidiNote) notes[i], octave, channel, velocity);
		}
	}

  return midinotes;

}
