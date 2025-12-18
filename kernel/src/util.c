#include "util.h"
#include <stdint.h>

// gcc and clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// implement them as the c specification mandates.
// do NOT remove or rename these functions, or stuff will eventually break!

void *memcpy(void *restrict dst, const void *restrict src, size_t n) {
  uint8_t *restrict pdst = (uint8_t *restrict) dst;
  const uint8_t *restrict psrc = (const uint8_t *restrict) src;
  for (size_t i = 0; i < n; i++)
    pdst[i] = psrc[i];
  return dst;
}

void *memset(void *s, int c, size_t n) {
  uint8_t *p = (uint8_t *) s;
  for (size_t i = 0; i < n; i++)
    p[i] = (uint8_t) c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  uint8_t *pdst = (uint8_t *) dst;
  const uint8_t *psrc = (const uint8_t *) src;

  if (src > dst) {
    for (size_t i = 0; i < n; i++) {
      pdst[i] = psrc[i];
    }
  } else if (src < dst) {
    for (size_t i = n; i > 0; i--) {
      pdst[i - 1] = psrc[i - 1];
    }
  }

  return dst;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const uint8_t *p1 = (const uint8_t *) s1;
  const uint8_t *p2 = (const uint8_t *) s2;

  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) {
      return p1[i] < p2[i] ? -1 : 1;
    }
  }

  return 0;
}

// halt and catch fire
__attribute__((noreturn))
void hcf(void) {
  for (;;)
    asm ("hlt");
}

void qprint(const char *s) {
  while (*s)
    asm ("out %0, $0xe9" : : "a"(*s++));
}
