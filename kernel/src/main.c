#include <stddef.h>
#include <limine.h>
#include "util.h"

// Set the base revision to 4, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
  .revision = 0
};

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;



/* -----------------
	 INTERRUPT STUFF!!
 - https://wiki.osdev.org/Interrupts_Tutorial
   ----------------- */

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
void exception_handler(void) {
	qprint("exception caught");
	__asm__ volatile ("cli; hlt"); // hang!
}

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
	idt_entry_t *descriptor = &idt[vector];
	descriptor->isr_low    = (uint64_t)isr & 0xFFFF;
	descriptor->kernel_cs  = 0;
	descriptor->ist        = 0;
	descriptor->attributes = flags;
	descriptor->isr_mid    = ((uint64_t)isr) >> 16 & 0xFFFF;
	descriptor->isr_high   = ((uint64_t)isr) >> 32 & 0xFFFFFFFF;
	descriptor->reserved   = 0;
}

#define IDT_MAX_DESCRIPTORS 32

extern void *isr_stub_table[];

void idt_init() {
	idtr.base = (uintptr_t)&idt[0];
	idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

	for (uint8_t vector = 0; vector < 32; vector++) {
		idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
	}

	__asm__ volatile ("lidt %0" : : "m"(idtr)); // load the idt
	__asm__ volatile ("sti"); // set the interrupt flag
}




/* -----------------
	 PIC stuff
 - https://wiki.osdev.org/8259_PIC#Programming_the_PIC_chips
   ----------------- */

// https://wiki.osdev.org/Inline_Assembly/Examples#I/O_access
static inline void outb(uint16_t port, int8_t val) {
  __asm__ volatile ("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline void io_wait(void) {
	outb(0x80, 0);
}


// ports
#define PIC1          0x20 // base address for master PIC
#define PIC2          0xA0 // base address for slave PIC
#define PIC1_COMMAND  PIC1
#define PIC1_DATA    (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA    (PIC2+1)

#define PIC_EOI 0x20 // end-of-interrupt command code

// if the interrupt came from the slave (irq >= 8), we send eoi to both master and slave
// otherwise, we only send to the master
void PIC_sendEOI(uint8_t irq) {
	if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);
	outb(PIC1_COMMAND, PIC_EOI);
}


// reinitialize the PIC controllers, giving them specified vector offsets
// ICW = initialization word

#define ICW1_ICW4      0x01 // indicates that icw4 (extra info about the environment) will be present
#define ICW1_SINGLE    0x02 // single (cascade) mode
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL     0x08 // level triggered (edge) mode
#define ICW1_INIT      0x10 // initialization

#define ICW4_8086       0x01 // 8086/88 (mcs-80/85) mode
#define ICW4_AUTO       0x02 // auto (normal) end-of-interrupt
#define ICW4_BUF_SLAVE  0x08 // buffered mode/slave
#define ICW4_BUF_MASTER 0x0c // buffered mode/master
#define ICW4_SFNM       0x10 // specify fully nested (not)

#define CASCADE_IRQ 2

// vectors on the master become offset1..offset1+7, on the slave become offset2..offset2+7
void PIC_remap(int offset1, int offset2) {

	// icw1: start the initialization sequence
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();

	outb(PIC1_DATA, offset1); // icw2: master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2); // icw2: slave PIC vector offset
	io_wait();

	outb(PIC1_DATA, 1 << CASCADE_IRQ); // icw3: tell master PIC that there is a slave PIC at IRQ2
	io_wait();
	outb(PIC2_DATA, 2);                // icw3: tell slave PIC its cascade identity (0000 0010)
	io_wait();

  // icw4: have the PICs use 8086 mode (and not 8080 mode)
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// unmask both PICs
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}





// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
  // Ensure the bootloader actually understands our base revision (see spec).
  if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
    hcf();
  }

	PIC_remap(0x00, 0x08);
	idt_init();

  // Ensure we got a framebuffer.
  if (framebuffer_request.response == NULL
   || framebuffer_request.response->framebuffer_count < 1) {
    hcf();
  }

  // Fetch the first framebuffer.
  struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

  // Note: we assume the framebuffer model is RGB with 32-bit pixels.
  for (size_t i = 0; i < 100; i++) {
    volatile uint32_t *fb_ptr = framebuffer->address;
    fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
  }

  qprint("hello qemu");

  // We're done, just hang...
  hcf();
}
