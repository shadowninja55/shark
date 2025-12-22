#include "timer.h"
#include "util.h"

#define PIT_FREQ 1193182
#define DATA 0x40
#define CMD  0x43

static uint64_t ticks = 0;

// https://wiki.osdev.org/Programmable_Interval_Timer#Frequency_Dividers
void timer_set_freq(uint32_t hz) {
  uint16_t div = PIT_FREQ / hz;  

  outb(CMD, 0b00110100);   // channel 0, lobyte / hibyte, rate generator, 16-bit binary
  outb(DATA, div & 0xff);  // lobyte
  outb(DATA, div >> 8);    // hibyte
}

void timer_trigger() {
  ticks++;
}

uint64_t timer_get() {
  return ticks;
}
