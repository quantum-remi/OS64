#include "gdt.h"

__attribute__((aligned(0x1000)))
static struct gdt_entry gdt[7] = {0};
static struct tss_entry tss = {0};

static struct gdt_ptr gdt_ptr;

void gdt_set_entry(int index, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran) {
    gdt[index].base_low = base & 0xFFFF;
    gdt[index].base_mid = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;
    
    gdt[index].limit_low = limit & 0xFFFF;
    gdt[index].granularity = (limit >> 16) & 0x0F;
    
    gdt[index].granularity |= gran & 0xF0;
    gdt[index].access = access;
}

void gdt_init(void) {
    // Null descriptor
    gdt_set_entry(0, 0, 0, 0, 0);
    
    // Kernel code segment
    gdt_set_entry(1, 0, 0, 0x9A, 0xA0);
    
    // Kernel data segment
    gdt_set_entry(2, 0, 0, 0x92, 0xC0);
    
    // User code segment
    gdt_set_entry(3, 0, 0, 0xFA, 0xA0);
    
    // User data segment
    gdt_set_entry(4, 0, 0, 0xF2, 0xC0);
    
    // TSS descriptor (index 5)
    uint64_t tss_base = (uint64_t)&tss;
    uint32_t tss_limit = sizeof(tss);
    
    gdt[5].limit_low = tss_limit & 0xFFFF;
    gdt[5].base_low = tss_base & 0xFFFF;
    gdt[5].base_mid = (tss_base >> 16) & 0xFF;
    gdt[5].access = 0x89; // Present, DPL 0, Available TSS
    gdt[5].granularity = (tss_limit >> 16) & 0x0F;
    gdt[5].base_high = (tss_base >> 24) & 0xFF;
    gdt[5].granularity |= 0x40; // 64-bit
    
    gdt_ptr.limit = sizeof(gdt) * 8 - 1;
    gdt_ptr.base = (uint64_t)gdt;
    
    // Load GDT and TSS
    gdt_load((uint64_t)&gdt_ptr);
    
    // Load TSS register
    __asm__ volatile("ltr %%ax" : : "a" (0x28 | 0)); // 0x28 = 5th entry (0-based * 8)
}
