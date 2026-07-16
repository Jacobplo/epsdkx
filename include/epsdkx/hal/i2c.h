#ifndef _EPSDKX_HAL_I2C_H
#define _EPSDKX_HAL_I2C_H

#include <epsdkx/common/i2c.h>

#include <stdint.h>


int hal_i2c_init(i2c_channel_t channel, i2c_mode_e mode, uint16_t address);

const i2c_pins_s *hal_i2c_get_pins(i2c_channel_t channel);

int hal_i2c_putn(i2c_channel_t channel, uint8_t *tx, uint16_t n, uint16_t slave_address);

int hal_i2c_getn(i2c_channel_t master, uint8_t *tx, uint16_t n, uint16_t slave_address);

int hal_i2c_get(i2c_channel_t channel, uint8_t *rx);

#endif
