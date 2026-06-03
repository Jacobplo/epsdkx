#include <stdint.h>
#include <stdbool.h>

#include "hal/gpio.h"
#include "hal/common/halstatus.h"

#include "stm32f1xx.h"


typedef struct {
  bool supported; 
  uint8_t mode_bits;
  uint8_t cnf_bits;
} gpio_config_entry_t;

static const gpio_config_entry_t gpio_config_map[GPIO_MODE_COUNT] = {
  [GPIO_IN_ANALOG]           = { .supported = true, .mode_bits = 0x0, .cnf_bits = 0x0 },
  [GPIO_IN_FLOATING]         = { .supported = true, .mode_bits = 0x0, .cnf_bits = 0x1 },
  [GPIO_IN_PULL_UP_DOWN]     = { .supported = true, .mode_bits = 0x0, .cnf_bits = 0x2 },
  [GPIO_OUT_PUSH_PULL]       = { .supported = true, .mode_bits = 0x2, .cnf_bits = 0x0 },
  [GPIO_OUT_OPEN_DRAIN]      = { .supported = true, .mode_bits = 0x2, .cnf_bits = 0x1 },
  [GPIO_OUT_ALT_PUSH_PULL]   = { .supported = true, .mode_bits = 0x2, .cnf_bits = 0x2 },
  [GPIO_OUT_ALT_OPEN_DRAIN]  = { .supported = true, .mode_bits = 0x2, .cnf_bits = 0x3 },
};

#define GPIO(port) ((GPIO_TypeDef *) (GPIOA_BASE + (uintptr_t)(0x400 * ((port) - 'A'))))

hal_status_t gpio_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN |
                  RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN;

  return HAL_OK;
}

hal_status_t gpio_configure(gpio_pin_t pin, gpio_mode_t mode) {
  if (mode >= GPIO_MODE_COUNT) return HAL_ERR_INVALID_ARG;
  if (!gpio_config_map[mode].supported) return HAL_ERR_INVALID_ARG;

  GPIO_TypeDef *gpio = GPIO(pin.port_pin.port);
  int8_t pin_num = pin.port_pin.pin;

  gpio_config_entry_t cfg = gpio_config_map[mode];

  // Pins 0-7
  if(pin_num < 8) {
    gpio->CRL &= ~(0xf << (pin_num * 4));
    gpio->CRL |= (uint32_t)(((cfg.cnf_bits << 2) | cfg.mode_bits) << (pin_num * 4));
  }

  // Pins 8-15
  else {
    gpio->CRH &= ~(0xf << ((pin_num - 8) * 4));
    gpio->CRH |= (uint32_t)(((cfg.cnf_bits << 2) | cfg.mode_bits) << ((pin_num - 8) * 4));
  }

  return HAL_OK;
}

hal_status_t gpio_write(gpio_pin_t pin, gpio_state_t state) {
  GPIO_TypeDef *gpio = GPIO(pin.port_pin.port);

  gpio->BSRR = (uint32_t)(0x1 << pin.port_pin.pin) << (state == GPIO_HIGH ? 0 : 16);

  return HAL_OK;
}

hal_status_t gpio_read(gpio_pin_t pin, gpio_state_t *ret_state) {
  GPIO_TypeDef *gpio = GPIO(pin.port_pin.port);

  *ret_state = gpio->IDR & (0x1 << pin.port_pin.pin) ? GPIO_HIGH : GPIO_LOW;

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
