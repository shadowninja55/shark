#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void hcf(void);
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);
void _putchar(char c);

#endif
