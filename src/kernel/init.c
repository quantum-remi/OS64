#include <stdint.h>
#include <stddef.h>

#include "string.h"
#include "serial.h"
#include "gop.h"

#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3)

__attribute__((used, section(".limine_requests")))
volatile struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST, 
  .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

struct gop_context gop_ctx;

void kmain(void)
{
  if (LIMINE_BASE_REVISION_SUPPORTED == false)
  {
    __asm__ volatile("hlt");
  }

  if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
  {
    __asm__ volatile("hlt");
  }


  if (serial_init() != 0) {
    serial_printf("Serial initialization failed.\n");
    __asm__ volatile("hlt");
  }

  serial_printf("=== Boot Sequence Started ===\n");

  gop_init(&gop_ctx);
  gop_clear(&gop_ctx);
  gop_draw_string(&gop_ctx, "GOP Driver Active\nSystem Ready");


  for(;;)
  {
    __asm__ volatile("hlt");
  }
}
