#ifndef _EPSDKX_COMMON_UART_H
#define _EPSDKX_COMMON_UART_H

#include <epsdkx/common/gpio.h>

#include <stdint.h>


typedef uint8_t uart_channel_t;
#define UART(n) (uart_channel_t)(n)

typedef struct uart_pins_s {
  gpio_pin_u tx;
  gpio_pin_u rx;
} uart_pins_s;

#endif
