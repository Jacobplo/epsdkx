#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <epsdkx/hal/gpio.h>

#include "stm32f1xx.h"


typedef struct hal_gpio_config_s {
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

#define GPIO(p) ((GPIO_TypeDef *) (GPIOA_BASE + (uintptr_t)(0x400 * ((p) - 'A'))))

void hal_gpio_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN |
                  RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN;
}

int hal_gpio_configure(hal_gpio_pin_u *pin, hal_gpio_mode_e mode) {
  if (mode >= HAL_GPIO_MODE_COUNT) return -EINVAL;
  if (!gpio_config_map[mode].supported) return -EINVAL;

  GPIO_TypeDef *gpio = GPIO(pin->port_pin.port);
  int8_t pin_num = pin->port_pin.pin;

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

  return 0;
}

void hal_gpio_write(hal_gpio_pin_u *pin, hal_gpio_state_e state) {
  GPIO_TypeDef *gpio = GPIO(pin->port_pin.port);

  gpio->BSRR = (uint32_t)(0x1 << pin->port_pin.pin) << (state == HAL_GPIO_HIGH ? 0 : 16);
}

hal_gpio_state_e hal_gpio_read(hal_gpio_pin_u *pin) {
  GPIO_TypeDef *gpio = GPIO(pin->port_pin.port);

  return gpio->IDR & (0x1 << pin->port_pin.pin) ? HAL_GPIO_HIGH : HAL_GPIO_LOW;
};

void hal_gpio_toggle(hal_gpio_pin_u *pin) {
  hal_gpio_state_e state = hal_gpio_read(pin);

  state == HAL_GPIO_HIGH ? hal_gpio_write(pin, HAL_GPIO_LOW) : hal_gpio_write(pin, HAL_GPIO_HIGH);
}
