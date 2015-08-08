#include "dhaum.h"

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

// Binders
DhaumBinderConf binders_configuration[] = {

  DhaumBinderConf(
    chat | pelote,
    ronronnement
  ),

  DhaumBinderConf(
    casserole | chat,
    coup_de_casserole
  ),

  DhaumBinderConf(
    casserole | pelote,
    cuisson
  ),

  DhaumBinderConf(
    casserole | echelle,
    coq
  ),

  DhaumBinderConf(
    casserole | chat | pelote,
    nyan
   ),

};

DhaumBinderData bindersd[sizeof(binders_configuration)/sizeof(*binders_configuration)];
uint16_t binders_size = (sizeof(binders_configuration)/sizeof(*binders_configuration));
DhaumBinderConf * binders_conf = binders_configuration;
DhaumBinderData * binders_data = bindersd;
