#include <epsdkx/gpio.h>

#include <epsdkx/hal/gpio.h>
#include <epsdkx/common/gpio.h>


void gpio_init(void) {
  hal_gpio_init();
}

int gpio_configure(gpio_pin_u *pin, gpio_mode_e mode) {
  return hal_gpio_configure(pin, mode);
}

void gpio_write(gpio_pin_u *pin, gpio_state_e state) {
  hal_gpio_write(pin, state);
}

gpio_state_e gpio_read(gpio_pin_u *pin) {
  return hal_gpio_read(pin);
}

void gpio_toggle(gpio_pin_u *pin) {
  hal_gpio_toggle(pin);
}

