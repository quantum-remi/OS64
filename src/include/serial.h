#pragma once

#include <stdint.h>

int serial_init();
void serial_putc(char c);
void serial_printf(const char *format, ...);
