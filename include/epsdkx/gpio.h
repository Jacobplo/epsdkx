#ifndef _EPSDKX_GPIO_H
#define _EPSDKX_GPIO_H

#include <epsdkx/common/gpio.h>


void gpio_init(void);

int gpio_configure(gpio_pin_u *pin, gpio_mode_e mode);

void gpio_write(gpio_pin_u *pin, gpio_state_e state);

gpio_state_e gpio_read(gpio_pin_u *pin);

void gpio_toggle(gpio_pin_u *pin);

#endif
