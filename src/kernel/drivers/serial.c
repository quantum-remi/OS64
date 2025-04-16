#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#include "io.h"
#include "printf.h"

#define COM1 0x3F8

int serial_init()
{
  outb(COM1 + 1, 0x00);
  outb(COM1 + 3, 0x80);
  outb(COM1 + 0, 0x03);
  outb(COM1 + 1, 0x00);
  outb(COM1 + 3, 0x03);
  outb(COM1 + 2, 0xC7);
  outb(COM1 + 4, 0x0B);
  outb(COM1 + 4, 0x1E);
  outb(COM1 + 0, 0xAE);

  if (inb(COM1 + 0) != 0xAE)
  {
    return 1;
  }

  outb(COM1 + 4, 0x0F);
  return 0;
}

void serial_putc(char c)
{
  while ((inb(COM1 + 5) & 0x20) == 0)
    ; // wait for tx buffer
  outb(COM1, c);
}

void serial_puts(const char *str)
{
  while (*str)
  {
    serial_putc(*str++);
  }
}

void serial_printf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vprintf_(format, args);
  va_end(args);
}

void _putchar(char c)
{
  serial_putc(c);
}
