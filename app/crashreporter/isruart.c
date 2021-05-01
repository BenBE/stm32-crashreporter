#include "crashreporter/isruart.h"

#include <stdint.h>

#include "stm32l4xx_ll_usart.h"

static USART_TypeDef* isruart = USART2;

static void internal_uart_putc(USART_TypeDef *u, char c) {
    //Assume the USART was initialized before ...
    if(!LL_USART_IsEnabled(u)) {
        return;
    }

    // Disable interrupt handling
    LL_USART_DisableIT_TC(u);
    LL_USART_DisableIT_TXE(u);

    while(!LL_USART_IsActiveFlag_TXE(u)) {
        // Wait for transmitter to become ready ...
    }

    LL_USART_TransmitData8(u, (uint8_t)c);

    while(!LL_USART_IsActiveFlag_TC(u)) {
        // Wait for byte to be sent
    }

    LL_USART_ClearFlag_TC(u);

    LL_USART_DisableIT_TXE(u);
    LL_USART_DisableIT_TC(u);
}

void cr_uart_putc(char c) {
    internal_uart_putc(isruart, c);
}

void cr_uart_puts(const char *s) {
    for(; *s; s++) {
        cr_uart_putc(*s);
    }
}

void cr_uart_putbuf(const char *s, size_t len) {
    for(; len; len--, s++) {
        cr_uart_putc(*s);
    }
}
