#ifndef _EPSDKX_UART_H
#define _EPSDKX_UART_H

#include <epsdkx/common/uart.h>

#include <stdint.h>


int uart_init(uart_channel_t channel, uint32_t baud_rate);

const uart_pins_s *uart_get_pins(uart_channel_t channel);

void uart_putc(uart_channel_t channel, char chr);

void uart_write(uart_channel_t channel, const char *str);

char uart_getc(uart_channel_t channel);

#endif
