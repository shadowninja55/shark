#ifndef RANDOM_H
#define RANDOM_H

// https://nuclear.llnl.gov/CNP/rng/rngman/node4.html

#include <stdint.h>
#include "random.h"

static uint64_t next = 1;

uint64_t rand(void) {
  next = 2862933555777941757 * next + 3037000493;
  return next;
}

void srand(uint64_t seed) {
  next = seed;
}

#endif
