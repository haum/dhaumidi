#include "dhaum.h"

enum serial_debug_flags {
  DEBUG_NONE = (1 << 0),
  DEBUG_RAW_CODES = (1 << 1),
  DEBUG_FILTERx22 = (1 << 2),
  DEBUG_FILTERED_TOUCHES = (1 << 3),
};

uint8_t serial_debug = DEBUG_FILTERED_TOUCHES;

long long loop_rdv = 0;

#define PADSNB (12)
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
    DhaumBinder & binder = binders[i];
    touchstatus_e touched = (binder.bits == pushed) ? TOUCHED : UNTOUCHED;
    if (touched == TOUCHED)
      binder.setTouched(TOUCHED);
  }
}

void setup() {
  // Reset pads
  for (int i = 0; i < PADSNB; ++i) {
    pinMode(L[i], INPUT);
    digitalWrite(L[i], LOW);
  }
  pinMode(L24, OUTPUT);

  // Reset binders
  init_binders();
  for (int i = 0; i < BINDERNB; ++i) {
    binders[i].setTouched(UNTOUCHED);
    binders[i].setDebounce(0);
    binders[i].setTouchedFiltered(UNTOUCHED);
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
    DhaumBinder & binder = binders[i];

    // Apply filter
    int8_t debounce = binder.getDebounce();
    debounce += (binder.touched() == UNTOUCHED) ? -1 : 1;
    if (debounce > MAXDEBOUNCE)
      debounce = MAXDEBOUNCE;
    if (debounce < 0)
      debounce = 0;

    // Reset touched status for next loop
    binder.setTouched(UNTOUCHED);

    // Debug filter 22h
    print_hex("Filer22h ", binder.getDebounce(), ((serial_debug & DEBUG_FILTERx22) && (binder.bits == 0x22)));

    // Trigger events
    if (debounce == MAXDEBOUNCE && binder.touchedFiltered() == UNTOUCHED) {
      MIDIUSB.note(1, binder.midi.getNote(), binder.midi.getOctave(), binder.midi.getChannel(), binder.midi.getVelocity());
      binder.setTouchedFiltered(TOUCHED);
      print_hex("On ", binder.bits, (serial_debug & DEBUG_FILTERED_TOUCHES));

    } else if (debounce == 0 && binder.touchedFiltered() != UNTOUCHED) {
      MIDIUSB.note(0, binder.midi.getNote(), binder.midi.getOctave(), binder.midi.getChannel(), binder.midi.getVelocity());
      binder.setTouchedFiltered(UNTOUCHED);
      print_hex("Off ", binder.bits, (serial_debug & DEBUG_FILTERED_TOUCHES));
    }
    binder.setDebounce(debounce);
  }

  // Wait for next loop
  while(loop_rdv > millis());
  loop_rdv = millis() + 10;

  // Blink at each loop
  digitalWrite(L24, !digitalRead(L24));

  // Flushes
  MIDIUSB.flush();
}

