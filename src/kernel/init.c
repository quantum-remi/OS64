#include <stdint.h>
#include <stddef.h>

#include "string.h"
#include "serial.h"

#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3)

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST, 
  .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

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

  struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

  serial_init();

  serial_printf("=== Boot Sequence Started ===\n");
  for (size_t i = 0; i < 100; i++)
  {
    volatile uint32_t *fb_ptr = framebuffer->address;
    fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
  }
  for(;;)
  {
    __asm__ volatile("hlt");
  }
}
