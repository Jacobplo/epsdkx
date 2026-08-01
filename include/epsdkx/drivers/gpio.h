/**
 * Public API for the GPIO driver.
 *
 * Unless otherwise stated, all functions that return int return a negative
 * errno on error, and 0 on success.
 */

#ifndef _EPSDKX_GPIO_H
#define _EPSDKX_GPIO_H

#include <epsdkx/common/gpio.h>


/**
 * Initializes all GPIO interfaces / ports.
 *
 * Should be called before using any other GPIO functions.
 */
void gpio_init(void);

/**
 * Configures a GPIO pin to user a given input or output mode.
 *
 * Returns -EINVAL if the mode is not supported on the hardware.
 */
int gpio_configure(gpio_pin_u *pin, gpio_mode_e mode);

/**
 * Sets a output GPIO pin to logic high or low.
 */
void gpio_write(gpio_pin_u *pin, gpio_state_e state);

/**
 * Reads and returns an input GPIO pin logic level.
 */
gpio_state_e gpio_read(gpio_pin_u *pin);

/**
 * Toggles the logic state of an output GPIO pin.
 */
void gpio_toggle(gpio_pin_u *pin);

#endif
