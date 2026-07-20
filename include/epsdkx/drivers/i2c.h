#ifndef _EPSDKX_I2C_H
#define _EPSDKX_I2C_H

#include <epsdkx/common/i2c.h>
#include <epsdkx/hal/i2c.h>

#include <stdint.h>


int i2c_init(i2c_channel_t channel, i2c_mode_e mode, uint16_t address);

static inline int i2c_init_master(i2c_channel_t channel) {
  return hal_i2c_init(channel, I2C_MASTER, 0);
}

static inline int i2c_init_slave(i2c_channel_t channel, uint16_t address) {
  return hal_i2c_init(channel, I2C_SLAVE, address);
}

const i2c_pins_s *i2c_get_pins(i2c_channel_t channel);

bool i2c_is_busy(i2c_channel_t channel);

int i2c_putn(i2c_channel_t channel, uint8_t *tx, uint16_t n, uint16_t slave_address);

int i2c_getn(i2c_channel_t channel, uint16_t n, uint16_t slave_address);

int i2c_get(i2c_channel_t channel, uint8_t *rx);

#endif
