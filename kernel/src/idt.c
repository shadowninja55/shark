#include "idt.h"
#include <stdbool.h>
#include <stdint.h>
#include "util.h"
#include "printf.h"
#include "pic.h"

typedef struct {
	uint16_t isr_low;    // lower 16 bits of the isr's address
	uint16_t kernel_cs;  // the GDT segment selector that the cpu loads into CS before calling the isr
	uint8_t  ist;        // the IST in the TSS  that the cpu will load into RSP [idk what this means]; set to zero for now
	uint8_t  attributes; // type and attributes
	uint16_t isr_mid;    // higher 16 bits of the lower 32 bits of the isr's address
	uint32_t isr_high;   // the higher 32 bits of the isr's address
	uint32_t reserved;   // set to zero
} __attribute__((packed)) idt_entry_t;

__attribute__((aligned(0x10)))
static idt_entry_t idt[256]; // aligned for performance

typedef struct {
	uint16_t limit;
	uint64_t base;
} __attribute__((packed)) idtr_t;
static idtr_t idtr; // idt register

__attribute__((noreturn))
void exception_handler(uint8_t vector, uint64_t error_code) {
  printf("[exception] vector: %u, error code: %lu\n", vector, error_code);
	asm volatile ("cli; hlt");
}

void irq_handler(uint8_t vector) {
  uint8_t irq = vector - 32;
  printf("[interrupt] irq: %u\n", irq);
  if (irq == 1) {
    uint8_t key = inb(0x60);
    printf("[keyboard] scancode: %u\n", key);
  }
  pic_send_eoi(irq);
}

static void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
	idt_entry_t *descriptor = &idt[vector];
	descriptor->isr_low     = (uint64_t) isr & 0xffff;
	descriptor->kernel_cs   = 0x28;
	descriptor->ist         = 0;
	descriptor->attributes  = flags;
	descriptor->isr_mid     = ((uint64_t) isr >> 16) & 0xffff;
	descriptor->isr_high    = ((uint64_t) isr >> 32) & 0xffffffff;
	descriptor->reserved    = 0;
}

#define IDT_MAX_DESCRIPTORS 48

extern void *isr_stub_table[];

void idt_init() {
	idtr.base = (uintptr_t) &idt[0];
	idtr.limit = (uint16_t) sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

	for (uint8_t vector = 0; vector < IDT_MAX_DESCRIPTORS; vector++) {
		idt_set_descriptor(vector, isr_stub_table[vector], 0x8e);
	}

  // load the idt
	asm volatile ("lidt %0" : : "m"(idtr));

  // set the interrupt flag
	asm volatile ("sti");
}
