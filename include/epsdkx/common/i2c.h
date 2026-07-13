#ifndef _EPSDKX_COMMON_I2C_H
#define _EPSDKX_COMMON_I2C_H

#include <epsdkx/common/gpio.h>

#include <stdint.h>


typedef uint8_t i2c_channel_t;
#define I2C(n) (i2c_channel_t)(n)

typedef struct i2c_pins_s {
  gpio_pin_u sda;
  gpio_pin_u scl;
} i2c_pins_s;

typedef enum i2c_mode_e {
  I2C_MASTER,
  I2C_SLAVE
} i2c_mode_e;

#endif
