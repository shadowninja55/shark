#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_set_freq(uint32_t hz);
void timer_tick(void);
uint64_t timer_get(void);

#endif
