#ifndef PIC_H
#define PIC_H

#include <stdint.h>

void pic_send_eoi(uint8_t irq);
void pic_remap(int master_offset, int slave_offset);

#endif
