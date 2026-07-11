#ifndef _EPSDKX_HAL_UART_H
#define _EPSDKX_HAL_UART_H

#include <epsdkx/common/uart.h>

#include <stdint.h>


int hal_uart_init(uart_channel_t channel, uint32_t baud_rate);

const uart_pins_s *hal_uart_get_pins(uart_channel_t channel);

void hal_uart_put(uart_channel_t channel, uint8_t tx);

int hal_uart_get(uart_channel_t channel, uint8_t *rx);

#endif
