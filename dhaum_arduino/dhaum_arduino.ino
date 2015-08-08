#include "dhaum.h"

#define MAX_BINDERS 5
DhaumBinderData bindersd[MAX_BINDERS];
DhaumBinderIndex binders_size = MAX_BINDERS;
DhaumBinderData * binders_data = bindersd;

// Objects
static DhaumObject casserole(1);
static DhaumObject pelote(2);
static DhaumObject chat(6);
static DhaumObject echelle(7);

// Sounds
static DhaumMidi ronronnement(MidiNote_C);
static DhaumMidi coup_de_casserole(MidiNote_D);
static DhaumMidi nyan(MidiNote_E);
static DhaumMidi cuisson(MidiNote_G);
static DhaumMidi coq(MidiNote_B);

DhaumBinderConf get_binderconf_nr(DhaumBinderIndex i) {
  switch (i) {
    case 0:
      return DhaumBinderConf(chat | pelote, ronronnement);
    case 1:
      return DhaumBinderConf(casserole | chat, coup_de_casserole);
    case 2:
      return DhaumBinderConf(casserole | pelote, cuisson);
    case 3:
      return DhaumBinderConf(casserole | echelle, coq);
    case 4:
      return DhaumBinderConf(casserole | chat | pelote, nyan);
    default:
      return DhaumBinderConf(DhaumObject(0xFFFF), DhaumMidi(MidiNote_C)); // dummy
  }
}
