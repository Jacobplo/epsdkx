/**
* Internal contract for HAL GPIO implementation
*
* Unless otherwise stated, all functions that return int should return a
* negative errno on error, and 0 on success.
*/

#ifndef _EPSDKX_HAL_GPIO_H
#define _EPSDKX_HAL_GPIO_H

#include <epsdkx/common/gpio.h>

/**
 * Should initialize all available GPIO interfaces / ports. No pin
 * configuration should be handled here.
 *
 * It is expected that this function be called before any other gpio function
 * at the user-level.
 */
void hal_gpio_init(void);

/**
 * Should configure a GPIO pin in a given input or output mode, such that it
 * can be set (in output mode) or read from (in input mode) immediately after.
 *
 * Should perform validation on mode in the case that the hardware does not
 * support all modes in gpio_mode_e, and return appropraite (negative) errnos
 * where necessary.
 */
int hal_gpio_configure(gpio_pin_u *pin, gpio_mode_e mode);

/**
 * Should set a GPIO pin in and output mode to the provided logic state.
 *
 * Should not perform any validation to confirm the pin is in the correct
 * mode; this is a user-level responsibility.
 */
void hal_gpio_write(gpio_pin_u *pin, gpio_state_e state);

/**
 * Should read and return logic state of a GPIO pin in an input mode.
 *
 * Should not perform any validation to confirm the pin is in the correct
 * mode; this is a user-level responsibility.
 */
gpio_state_e hal_gpio_read(gpio_pin_u *pin);

#endif
