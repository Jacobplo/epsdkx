#ifndef _EPSDKX_HAL_GPIO_H
#define _EPSDKX_HAL_GPIO_H

#include <epsdkx/common/gpio.h>

void hal_gpio_init(void);

int hal_gpio_configure(gpio_pin_u *pin, gpio_mode_e mode);

void hal_gpio_write(gpio_pin_u *pin, gpio_state_e state);

gpio_state_e hal_gpio_read(gpio_pin_u *pin);

void hal_gpio_toggle(gpio_pin_u *pin);

#endif
