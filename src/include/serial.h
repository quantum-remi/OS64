#pragma once

#include <stdint.h>

void serial_init();
void serial_putc(char c);
void serial_printf(const char* str);
