#define STB_SPRINTF_IMPLEMENTATION

#include <limine.h>
#include <stdbool.h>
#include "printf.h"
#include "util.h"
#include "idt.h"
#include "pic.h"
#include "timer.h"
#include "video.h"
#include "clock.h"

// Set the base revision to 4, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

// kernel entry point.
// if renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
  if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
    hcf();
  }

  printf("[info] kernel booted\n");

  video_init();

  uint64_t w = video_width();
  uint64_t h = video_height();

  uint64_t m = 0xffffff / (w * h);
  for (uint64_t y = 0; y < h; y++) {
    for (uint64_t x = 0; x < w; x++) {
      uint32_t c = (uint32_t) ((y * w + x) * m);
      video_set(x, y, c);
    }
  }
  
	pic_remap(32, 32 + 8);
  clock_init();
  timer_set_freq(1000);
	idt_init();

  hcf();
}
