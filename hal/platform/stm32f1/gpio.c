#include <stdint.h>

#include "hal/gpio.h"
#include "hal/common/halstatus.h"

#include "stm32f1xx.h"

#define GPIO(port) ((GPIO_TypeDef *) (GPIOA_BASE + (uintptr_t)(0x400 * ((port) - 'A'))))

hal_status_t gpio_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN |
                  RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN;

  return HAL_OK;
}

hal_status_t gpio_configure(gpio_pin_t pin, gpio_mode_t mode, gpio_configuration_t configuration) {
  GPIO_TypeDef *gpio = GPIO(pin.port_pin.port);
  int8_t pin_num = pin.port_pin.pin;

  // Pins 0-7
  if(pin_num < 8) {
    gpio->CRL &= ~(15u << (pin_num * 4));
    gpio->CRL |= (uint32_t)(((configuration << 2) | mode) << (pin_num * 4));
  }

  // Pins 8-15
  else {
    gpio->CRH &= ~(15u << ((pin_num - 8) * 4));
    gpio->CRH |= (uint32_t)(((configuration << 2) | mode) << ((pin_num - 8) * 4));
  }

  return HAL_OK;
}

hal_status_t gpio_write(gpio_pin_t pin, gpio_state_t state) {
  GPIO_TypeDef *gpio = GPIO(pin.port_pin.port);

  gpio->BSRR = (uint32_t)(1u << pin.port_pin.pin) << (state ? 0 : 16);

  return HAL_OK;
}

hal_status_t gpio_read(gpio_pin_t pin, gpio_state_t *ret_state) {
  GPIO_TypeDef *gpio = GPIO(pin.port_pin.port);

  *ret_state = gpio->IDR & (1u << pin.port_pin.pin) ? GPIO_HIGH : GPIO_LOW;

  return HAL_OK;
};

hal_status_t gpio_toggle(gpio_pin_t pin) {
  hal_status_t status;

  gpio_state_t state;
  status = gpio_read(&state, pin);

  if (status == HAL_OK) {
    status = gpio_write(pin, state);
  }

  return status;
}
