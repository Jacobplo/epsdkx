#ifndef _EPSDKX_HAL_UART_H
#define _EPSDKX_HAL_UART_H

#include <epsdkx/hal/gpio.h>

#include <stdint.h>


typedef uint8_t hal_uart_channel_t;
#define UART(n) (hal_uart_channel_t)(n)

typedef struct hal_uart_pins_s {
  hal_gpio_pin_u tx;
  hal_gpio_pin_u rx;
} hal_uart_pins_s;


int hal_uart_init(hal_uart_channel_t channel, uint32_t baud_rate);

const hal_uart_pins_s *hal_uart_get_pins(hal_uart_channel_t channel);

void hal_uart_putc(hal_uart_channel_t channel, char chr);

void hal_uart_write(hal_uart_channel_t channel, const char *str);

char hal_uart_getc(hal_uart_channel_t channel);

#endif
