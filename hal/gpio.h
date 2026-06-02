#ifndef EPSDKX_GPIO_H
#define EPSDKX_GPIO_H

#include <stdint.h>

#include "common/halstatus.h"

typedef union {
  struct {
    uint8_t port;
    uint8_t pin;
  } port_pin;
  uint16_t pin_id;
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
  GPIO_HIGH = 1
} gpio_state_t;

hal_status_t gpio_init(void);

hal_status_t gpio_configure(gpio_pin_t pin, gpio_mode_t mode, gpio_configuration_t configuration);

hal_status_t gpio_write(gpio_pin_t pin, gpio_state_t state);

hal_status_t gpio_read(gpio_pin_t pin, gpio_state_t *ret_state);

hal_status_t gpio_toggle(gpio_pin_t pin);

#endif
