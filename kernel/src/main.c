#define STB_SPRINTF_IMPLEMENTATION

#include <limine.h>
#include <stdbool.h>
#include <stddef.h>
#include "printf.h"
#include "util.h"
#include "idt.h"
#include "pic.h"

// Set the base revision to 4, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
  .revision = 0
};

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

  if (framebuffer_request.response == NULL
   || framebuffer_request.response->framebuffer_count < 1) {
    hcf();
  }

  struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

  // note: we assume the framebuffer model is rgb with 32-bit pixels
  for (size_t i = 0; i < 100; i++) {
    volatile uint32_t *fb_ptr = framebuffer->address;
    fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
  }

  // remap pic so that irq's don't conflict with cpu exception vectors
	pic_remap(32, 32 + 8);
	idt_init();
  hcf();
}
