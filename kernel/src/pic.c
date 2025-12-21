#include "pic.h"
#include "util.h"

// https://wiki.osdev.org/Inline_Assembly/Examples#I/O_access
static inline void io_wait(void) {
	outb(0x80, 0);
}

// ports
#define MASTER          0x20 // base address for master pic
#define SLAVE          0xa0 // base address for slave pic
#define MASTER_COMMAND  MASTER
#define MASTER_DATA    (MASTER+1)
#define SLAVE_COMMAND  SLAVE
#define SLAVE_DATA    (SLAVE+1)
#define PIC_EOI 0x20 // end-of-interrupt command code

// should be issued to pic chip at the end of an irq-based isr
// if the interrupt came from the slave (irq >= 8), we send eoi to both master and slave
// otherwise, we only send to the master
void pic_send_eoi(uint8_t irq) {
	outb(MASTER_COMMAND, PIC_EOI);
	if (irq >= 8) 
    outb(SLAVE_COMMAND, PIC_EOI);
}

// reinitialize the pic controllers, giving them specified vector offsets
// icw = initialization word

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

// vectors on the master become offset1..offset1+7
// on the slave become offset2..offset2+7
void pic_remap(int master_offset, int slave_offset) {

	// icw1: start the initialization sequence
	outb(MASTER_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(SLAVE_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();

	outb(MASTER_DATA, master_offset); // icw2: master pic vector offset
	io_wait();
	outb(SLAVE_DATA, slave_offset); // icw2: slave pic vector offset
	io_wait();

	outb(MASTER_DATA, 1 << CASCADE_IRQ); // icw3: tell master pic that there is a slave pic at irq2
	io_wait();
	outb(SLAVE_DATA, 2);                // icw3: tell slave pic its cascade identity (0000 0010)
	io_wait();

  // icw4: have the pic's use 8086 mode (and not 8080 mode)
	outb(MASTER_DATA, ICW4_8086);
	io_wait();
	outb(SLAVE_DATA, ICW4_8086);
	io_wait();

	// unmask both pic's
	outb(MASTER_DATA, 0);
	outb(SLAVE_DATA, 0);
}
