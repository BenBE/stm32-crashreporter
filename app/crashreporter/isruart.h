#pragma once

#include <stddef.h>

void cr_uart_putc(char c);
void cr_uart_puts(const char *s);
void cr_uart_putbuf(const char *s, size_t len);
