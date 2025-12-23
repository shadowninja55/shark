#include <stdint.h>
#include "printf.h"
#include "timer.h"
#include "keyboard.h"
#include "pic.h"
#include "clock.h"
#include "game.h"

#define IRQ_PIT      0
#define IRQ_KEYBOARD 1
#define IRQ_RTC      8

void irq_handler(uint8_t vector) {
  uint8_t irq = vector - 32;
  switch (irq) {
    case IRQ_PIT:
      timer_trigger();
      uint64_t ticks = timer_get();
      if (!(ticks % 100)) game_trigger();
      break;
    case IRQ_KEYBOARD:
			keyboard_trigger();
      break;
    case IRQ_RTC:
      clock_trigger();
      break;
    default:
      printf("[interrupt] irq: %u\n", irq);
  }
  pic_send_eoi(irq);
}

