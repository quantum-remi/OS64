#pragma once

#include <stdint.h>

struct idt_entry
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

void isr_handler(uint64_t vector, uint64_t error_code, uint64_t rip);

void divide_error_handler(void);
void general_protection_fault_handler(void);
void page_fault_handler(void);

void keyboard_handler(void);
void timer_handler(void);

void idt_init(void);
