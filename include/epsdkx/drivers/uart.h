#ifndef _EPSDKX_UART_H
#define _EPSDKX_UART_H

#include <epsdkx/common/uart.h>

#include <stdint.h>


int uart_init(uart_channel_t channel, uint32_t baud_rate);

const uart_pins_s *uart_get_pins(uart_channel_t channel);

void uart_put(uart_channel_t channel, uint8_t tx);

int uart_get(uart_channel_t channel, uint8_t *rx);

void uart_write(uart_channel_t channel, const char *str);

#endif
