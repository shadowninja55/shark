#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

void video_init(void);
uint64_t video_width(void);
uint64_t video_height(void);
void video_set(uint64_t x, uint64_t y, uint32_t color);
void video_fill(uint32_t color);

#endif
