#include <stdbool.h>
#include <limine.h>
#include "util.h"
#include "printf.h"

#define CMOS_INDEX 0x70
#define CMOS_DATA  0x71

__attribute__((used, section(".limine_requests")))
static volatile struct limine_date_at_boot_request date_req = {
  .id = LIMINE_DATE_AT_BOOT_REQUEST_ID,
  .revision = 0
};

static uint64_t unix_time;
static bool tock = false;

// https://wiki.osdev.org/RTC#Turning_on_IRQ_8
void clock_init(void) {
  if (!date_req.response) {
    printf("[fatal] no date at boot\n");
    hcf();
  }

  unix_time = date_req.response->timestamp;

  outb(CMOS_INDEX, 0x8b);
  io_wait();
  uint8_t prev = inb(CMOS_DATA);

  outb(CMOS_INDEX, 0x8b);
  io_wait();
  outb(CMOS_DATA, prev | (1 << 6)); // irq 8 is bit 6

  outb(CMOS_INDEX, 0x8a);
  io_wait();
  prev = inb(CMOS_DATA);

  // corresponds to 2hz
  uint8_t div = 15;
  outb(CMOS_INDEX, 0x8a);
  io_wait();
  outb(CMOS_DATA, (prev & 0b11110000) | div); // divider is lower nibble
}

// https://wiki.osdev.org/RTC#Interrupts_and_Register_C
void clock_trigger(void) {
  outb(CMOS_INDEX, 0x8c);
  io_wait();
  uint8_t mask = inb(CMOS_DATA); 

  // periodic is bit 6
  if (mask & (1 << 6)) {
    unix_time += tock;
    if (tock) printf("[rtc] unix time: %llu\n", unix_time);
    tock = !tock;
  }
}
