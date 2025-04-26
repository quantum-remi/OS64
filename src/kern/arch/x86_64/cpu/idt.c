#include "idt.h"
#include "serial.h"

struct idt_entry idt[256];
struct idt_ptr idtr;

extern void *isr_stub_table[];

static void set_idt_gate(int num, void *handler, uint8_t flags)
{
    idt[num].base_low = (uint64_t)handler & 0xFFFF;
    idt[num].selector = 0x08;
    idt[num].ist = 0;
    idt[num].flags = flags;
    idt[num].base_mid = ((uint64_t)handler >> 16) & 0xFFFF;
    idt[num].base_high = ((uint64_t)handler >> 32) & 0xFFFFFFFF;
    idt[num].zero = 0;
}

void idt_init(void)
{
    idtr.base = (uint64_t)&idt;
    idtr.limit = sizeof(idt) - 1;

    // Set exception ISRs (0-31)
    for (int i = 0; i < 32; i++)
    {
        set_idt_gate(i, isr_stub_table[i], 0x8E);
    }

    // Set hardware IRQs (timer: 32, keyboard: 33)
    set_idt_gate(32, isr_stub_table[32], 0x8E);
    set_idt_gate(33, isr_stub_table[33], 0x8E);

    __asm__ volatile("lidt %0" : : "m"(idtr));
    serial_printf("IDT loaded at 0x%llx\n", idtr.base);
}
