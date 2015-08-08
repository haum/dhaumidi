#include "dhaum.h"

enum serial_debug_flags {
  DEBUG_NONE = (1 << 0),
  DEBUG_RAW_CODES = (1 << 1),
  DEBUG_FILTERx22 = (1 << 2),
  DEBUG_FILTERED_TOUCHES = (1 << 3),
};

uint8_t serial_debug = DEBUG_FILTERED_TOUCHES;

long long loop_rdv = 0;

#define PADSNB (sizeof(L)/sizeof(*L))
#define BINDERNB (binders_size)
#define MAXDEBOUNCE 3

void print_hex(const char * const str, DhaumBits hex, bool predicat) {
  if (!predicat) return;
  if (!Serial) return;
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

void notify_binders(DhaumBits pushed) {
  for (DhaumBinderIndex i = 0; i < BINDERNB; ++i) {
    DhaumBinderConf * binder_c = &binders_conf[i];
    touchstatus_e touched = ((binder_c->bits & binder_c->mask) == (pushed & binder_c->mask)) ? TOUCHED : UNTOUCHED;
    if (touched == TOUCHED) {
      DhaumBinderData * binder_d = &binders_data[i];
      binder_d->setTouched(TOUCHED);
    }
  }
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

void loop() {
  // For each pad
  for (int i = 1; i < PADSNB; ++i) {
    // Get snapshot of pads (with ith activated) and update binders' status
    pinMode(L[i], OUTPUT);
    DhaumBits pushed = get_pads_snapshot();
    digitalWrite(L[i], HIGH);
    pinMode(L[i], INPUT);
    digitalWrite(L[i], LOW);
    notify_binders(pushed);
    delay(1);

    // Debug raw codes
    print_hex("Code : ", pushed, ((serial_debug & DEBUG_RAW_CODES) && (pushed != (DhaumBits(1) << (i - 1)))));
  }

  // Debug raw codes (loop separator)
  print_hex("0==", 0, (serial_debug & DEBUG_RAW_CODES));

  // For each binder
  for (DhaumBinderIndex i = 0; i < BINDERNB; ++i) {
    DhaumBinderConf & binder_c = binders_conf[i];
    DhaumBinderData & binder_d = binders_data[i];

    // Apply filter
    int8_t debounce = binder_d.getDebounce();
    debounce += (binder_d.touched() == UNTOUCHED) ? -1 : 1;
    if (debounce > MAXDEBOUNCE)
      debounce = MAXDEBOUNCE;
    if (debounce < 0)
      debounce = 0;

    // Reset touched status for next loop
    binder_d.setTouched(UNTOUCHED);

    // Debug filter 22h
    print_hex("Filer22h ", binder_d.getDebounce(), ((serial_debug & DEBUG_FILTERx22) && (binder_c.bits == 0x22)));

    // Trigger events
    if (debounce == MAXDEBOUNCE && binder_d.touchedFiltered() == UNTOUCHED) {
      MIDIUSB.note(1, binder_c.midi.getNote(), binder_c.midi.getOctave(), binder_c.midi.getChannel(), binder_c.midi.getVelocity());
      binder_d.setTouchedFiltered(TOUCHED);
      print_hex("On ", binder_c.bits, (serial_debug & DEBUG_FILTERED_TOUCHES));

    } else if (debounce == 0 && binder_d.touchedFiltered() != UNTOUCHED) {
      MIDIUSB.note(0, binder_c.midi.getNote(), binder_c.midi.getOctave(), binder_c.midi.getChannel(), binder_c.midi.getVelocity());
      binder_d.setTouchedFiltered(UNTOUCHED);
      print_hex("Off ", binder_c.bits, (serial_debug & DEBUG_FILTERED_TOUCHES));
    }
    binder_d.setDebounce(debounce);
  }

  // Wait for next loop
  while(loop_rdv > millis());
  loop_rdv = millis() + 10;

  // Flushes
  MIDIUSB.flush();
}

