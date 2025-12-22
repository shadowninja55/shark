#include <limine.h>
#include "video.h"
#include "util.h"
#include "printf.h"

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request fb_req = {
  .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
  .revision = 0
};
static struct limine_framebuffer *fb;
static uint32_t *fb_ptr; 

void video_init(void) {
  if (!fb_req.response || fb_req.response->framebuffer_count < 1) {
    printf("[fatal] no framebuffer\n");
    hcf();
  }
  fb = fb_req.response->framebuffers[0];
  fb_ptr = fb->address;
}

uint64_t video_width(void) {
  return fb->width;
}

uint64_t video_height(void) {
  return fb->height;
}

void video_set(uint64_t x, uint64_t y, uint32_t color) {
  // note: we assume the framebuffer model is rgb with 32-bit pixels
  fb_ptr[y * (fb->pitch / 4) + x] = color;
}

void video_fill(uint32_t color) {
  for (uint64_t y = 0; y < fb->height; y++) {
    for (uint64_t x = 0; x < fb->width; x++) {
      video_set(x, y, color);
    }
  }
}
