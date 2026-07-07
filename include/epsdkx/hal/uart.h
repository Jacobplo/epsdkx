#ifndef _EPSDKX_HAL_UART_H
#define _EPSDKX_HAL_UART_H

#include <epsdkx/hal/gpio.h>

#include <stdint.h>


typedef uint8_t hal_uart_controller_t;

#define UART(n) (hal_uart_controller_t)((n) - 1)

typedef struct {
  hal_gpio_pin_u tx;
  hal_gpio_pin_u rx;
} hal_uart_pins_s;


int hal_uart_init(hal_uart_controller_t controller, uint32_t baud_rate);

#endif
