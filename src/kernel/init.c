#include <stdint.h>
#include <stddef.h>


#define LIMINE_NO_POINTERS
#include <limine.h>

static volatile struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST, 
  .revision = 0
};


void kmain(void)
{
  for(;;)
  {
    __asm__ volatile("hlt");
  }
}
