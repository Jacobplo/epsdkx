#ifndef _EPSDKX_HAL_I2C_H
#define _EPSDKX_HAL_I2C_H

#include <epsdkx/common/i2c.h>

#include <stdint.h>


int hal_i2c_init(i2c_channel_t channel, i2c_mode_e mode, uint16_t address);

static inline int hal_i2c_init_master(i2c_channel_t channel) {
  return hal_i2c_init(channel, I2C_MASTER, 0);
}

static inline int hal_i2c_init_slave(i2c_channel_t channel, uint16_t address) {
  return hal_i2c_init(channel, I2C_SLAVE, address);
}

const i2c_pins_s *hal_i2c_get_pins(i2c_channel_t channel);

int hal_i2c_put(i2c_channel_t channel, uint8_t tx);

int hal_i2c_get(i2c_channel_t channel, uint8_t *rx);

#endif
