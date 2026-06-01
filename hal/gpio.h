#ifndef STM32F1XX_GPIO_H

#define STM32F1XX_GPIO_H

#include <stdint.h>

#include "stm32f103xb.h"

// GPIO pin mode
enum {
  GPIO_INPUT = 0,
  GPIO_OUTPUT_10MHZ,
  GPIO_OUTPUT_2MHZ,
  GPIO_OUTPUT_50MHZ
};

// GPIO pin configuration
enum {
  GPIO_IN_ANALOG = 0,
  GPIO_IN_FLOATING,
  GPIO_IN_PULL_UP_DOWN
};
enum {
  GPIO_OUT_PUSH_PULL = 0,
  GPIO_OUT_OPEN_DRAIN,
  GPIO_OUT_ALT_PUSH_PULL,
  GPIO_OUT_ALT_OPEN_DRAIN
};

// GPIO State
enum {
  GPIO_LOW = 0,
  GPIO_HIGH
};

#define GPIO(port) ((GPIO_TypeDef *) (GPIOA_BASE + (uintptr_t)(0x400 * ((port) - 'A'))))
#define PIN(port, num) (((port) << 8) | (num)) // uint16_t
#define PIN_PORT(pin) ((pin) >> 8)
#define PIN_NUM(pin) (uint8_t)((pin) & 255u)

// Defines the input/output mode of the GPIO pin, with configuration for the type.
static inline void gpio_set(uint16_t pin, uint8_t mode, uint8_t configuration) {
  GPIO_TypeDef *gpio = GPIO(PIN_PORT(pin));
  uint8_t pin_num = PIN_NUM(pin);

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
}

// For an output GPIO pin, sets the output.
static inline void gpio_write(uint16_t pin, uint8_t state) {
  GPIO_TypeDef *gpio = GPIO(PIN_PORT(pin));
  uint8_t pin_num = PIN_NUM(pin);

  gpio->BSRR = (uint32_t)(1u << pin_num) << (state ? 0 : 16);
}

// For an input GPIO pin, reads the input.
static inline uint8_t gpio_read(uint16_t pin) {
  GPIO_TypeDef *gpio = GPIO(PIN_PORT(pin));
  uint8_t pin_num = PIN_NUM(pin);

  return (uint8_t)(gpio->IDR & (1u << pin_num));
}

static inline void gpio_toggle(uint16_t pin) {
  uint8_t state = gpio_read(pin);
  state ? gpio_write(pin, GPIO_LOW) : gpio_write(pin, GPIO_HIGH);
}


// Enable RCC clock for all GPIO ports.
static inline void gpio_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN
                | RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPEEN;
}


#endif
