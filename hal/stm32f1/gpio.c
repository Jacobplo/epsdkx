#include <stdint.h>
#include <stdbool.h>

#include "hal/gpio.h"
#include "hal/common/status.h"

#include "stm32f1xx.h"


typedef struct {
  bool supported; 
  uint8_t mode_bits;
  uint8_t cnf_bits;
} hal_gpio_config_s;

static const hal_gpio_config_s gpio_config_map[HAL_GPIO_MODE_COUNT] = {
  [HAL_GPIO_IN_ANALOG]           = { .supported = true, .mode_bits = 0x0, .cnf_bits = 0x0 },
  [HAL_GPIO_IN_FLOATING]         = { .supported = true, .mode_bits = 0x0, .cnf_bits = 0x1 },
  [HAL_GPIO_IN_PULL_UP_DOWN]     = { .supported = true, .mode_bits = 0x0, .cnf_bits = 0x2 },
  [HAL_GPIO_OUT_PUSH_PULL]       = { .supported = true, .mode_bits = 0x2, .cnf_bits = 0x0 },
  [HAL_GPIO_OUT_OPEN_DRAIN]      = { .supported = true, .mode_bits = 0x2, .cnf_bits = 0x1 },
  [HAL_GPIO_OUT_ALT_PUSH_PULL]   = { .supported = true, .mode_bits = 0x2, .cnf_bits = 0x2 },
  [HAL_GPIO_OUT_ALT_OPEN_DRAIN]  = { .supported = true, .mode_bits = 0x2, .cnf_bits = 0x3 },
};

#define GPIO(port) ((GPIO_TypeDef *) (GPIOA_BASE + (uintptr_t)(0x400 * ((port) - 'A'))))

hal_status_e hal_gpio_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN |
                  RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN;

  return HAL_STATUS_OK;
}

hal_status_e hal_gpio_configure(hal_gpio_pin_u pin, hal_gpio_mode_e mode) {
  if (mode >= HAL_GPIO_MODE_COUNT) return HAL_STATUS_ERR_INVALID_ARG;
  if (!gpio_config_map[mode].supported) return HAL_STATUS_ERR_INVALID_ARG;

  GPIO_TypeDef *gpio = GPIO(pin.port_pin.port);
  int8_t pin_num = pin.port_pin.pin;

  hal_gpio_config_s cfg = gpio_config_map[mode];

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

  return HAL_STATUS_OK;
}

hal_status_e hal_gpio_write(hal_gpio_pin_u pin, hal_gpio_state_e state) {
  GPIO_TypeDef *gpio = GPIO(pin.port_pin.port);

  gpio->BSRR = (uint32_t)(0x1 << pin.port_pin.pin) << (state == HAL_GPIO_HIGH ? 0 : 16);

  return HAL_STATUS_OK;
}

hal_status_e hal_gpio_read(hal_gpio_pin_u pin, hal_gpio_state_e *ret_state) {
  GPIO_TypeDef *gpio = GPIO(pin.port_pin.port);

  *ret_state = gpio->IDR & (0x1 << pin.port_pin.pin) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;

  return HAL_STATUS_OK;
};

hal_status_e hal_gpio_toggle(hal_gpio_pin_u pin) {
  hal_status_e status;

  hal_gpio_state_e state;
  status = hal_gpio_read(pin, &state);

  if (status == HAL_STATUS_OK) {
    status = hal_gpio_write(pin, state);
  }

  return status;
}
