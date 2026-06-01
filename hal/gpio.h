#ifndef EPSDKX_GPIO_H

#define EPSDKX_GPIO_H

#include <stdint.h>

#include "common/halstatus.h"

typedef union {
  struct {
    uint8_t port;
    uint8_t pin_num;
  };
  uint16_t pin_num_full;
} gpio_pin_t;

// GPIO pin mode
typedef enum {
  GPIO_INPUT,
  GPIO_OUTPUT_10MHZ,
  GPIO_OUTPUT_2MHZ,
  GPIO_OUTPUT_50MHZ
} gpio_mode_t;

// GPIO pin configuration
typedef enum {
  // Input
  GPIO_IN_ANALOG,
  GPIO_IN_FLOATING,
  GPIO_IN_PULL_UP_DOWN,

  // Output
  GPIO_OUT_PUSH_PULL,
  GPIO_OUT_OPEN_DRAIN,
  GPIO_OUT_ALT_PUSH_PULL,
  GPIO_OUT_ALT_OPEN_DRAIN
} gpio_configuration_t;

// GPIO State
typedef enum {
  GPIO_LOW = 0,
  GPIO_HIGH
} gpio_state_t;

static inline hal_status_t gpio_init(void);

static inline hal_status_t gpio_configure(gpio_pin_t pin, gpio_mode_t mode, gpio_configuration_t configuration);

static inline hal_status_t gpio_write(gpio_pin_t pin, gpio_state_t state);

static inline hal_status_t gpio_read(gpio_state_t *ret, gpio_pin_t pin);

static inline hal_status_t gpio_toggle(gpio_pin_t pin);

#endif
