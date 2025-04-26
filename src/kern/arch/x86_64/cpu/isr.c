#include "idt.h"
#include "serial.h"
#include "io.h"

#define UNUSED(x) (void)(x)

void isr_handler(uint64_t vector, uint64_t error_code, uint64_t rip)
{
    serial_printf("Exception/IRQ %llu (Error: 0x%llx) at RIP: 0x%llx\n",
                  vector, error_code, rip);

    if (vector == 32)
    {
        // static uint64_t ticks = 0;
        // ticks++;
        outb(0x20, 0x20);
        return;
    }
    if (vector == 33)
    {
        uint8_t scancode = inb(0x60);
        serial_printf("Keyboard input: 0x%x\n", scancode);
        outb(0x20, 0x20);
        return;
    }

    for (;;)
        __asm__ volatile("cli; hlt");
}
