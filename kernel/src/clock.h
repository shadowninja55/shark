#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

void clock_init(void);
void clock_trigger(void);
uint64_t clock_get(void);

#endif
