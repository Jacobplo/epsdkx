#ifndef _EPSDKX_HAL_GPIO_H
#define _EPSDKX_HAL_GPIO_H

#include <stdint.h>


typedef union {
  struct {
    uint8_t port;
    uint8_t pin;
  } port_pin;
  uint16_t pin_id;
} hal_gpio_pin_u;

// GPIO pin mode 
typedef enum {
  // Input
  HAL_GPIO_IN_FLOATING,
  HAL_GPIO_IN_PULL_UP_DOWN,
  HAL_GPIO_IN_ANALOG,

  // Output
  HAL_GPIO_OUT_PUSH_PULL,
  HAL_GPIO_OUT_OPEN_DRAIN,
  HAL_GPIO_OUT_ALT_PUSH_PULL,
  HAL_GPIO_OUT_ALT_OPEN_DRAIN,

  HAL_GPIO_MODE_COUNT
} hal_gpio_mode_e;

#define HAL_GPIO_IN  HAL_GPIO_IN_FLOATING;
#define HAL_GPIO_OUT HAL_GPIO_OUT_PUSH_PULL;

// GPIO State
typedef enum {
  HAL_GPIO_LOW,
  HAL_GPIO_HIGH
} hal_gpio_state_e;

void hal_gpio_init(void);

int hal_gpio_configure(hal_gpio_pin_u pin, hal_gpio_mode_e mode);

void hal_gpio_write(hal_gpio_pin_u pin, hal_gpio_state_e state);

hal_gpio_state_e hal_gpio_read(hal_gpio_pin_u pin);

void hal_gpio_toggle(hal_gpio_pin_u pin);

#endif
