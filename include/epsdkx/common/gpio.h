#ifndef _EPSDKX_COMMON_GPIO_H
#define _EPSDKX_COMMON_GPIO_H

#include <stdint.h>


typedef union gpio_pin_u {
  struct {
    uint8_t port;
    uint8_t pin;
  } port_pin;
  uint16_t pin_id;
} gpio_pin_u;

#define PORT_PIN(p, n) (gpio_pin_u){ .port_pin = { .port = (p), .pin = (n) } }
#define PIN_ID(n)      (gpio_pin_u){ .pin_id = (n) }

// GPIO pin mode 
typedef enum gpio_mode_e {
  // Input
  GPIO_IN_FLOATING,
  GPIO_IN_PULL_UP_DOWN,
  GPIO_IN_ANALOG,

  // Output
  GPIO_OUT_PUSH_PULL,
  GPIO_OUT_OPEN_DRAIN,
  GPIO_OUT_ALT_PUSH_PULL,
  GPIO_OUT_ALT_OPEN_DRAIN,

  GPIO_MODE_COUNT
} gpio_mode_e;

#define GPIO_IN  GPIO_IN_FLOATING
#define GPIO_OUT GPIO_OUT_PUSH_PULL

// GPIO State
typedef enum gpio_state_e {
  GPIO_LOW,
  GPIO_HIGH
} gpio_state_e;

#endif
