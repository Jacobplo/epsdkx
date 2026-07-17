#include <epsdkx/drivers/i2c.h>

#include <epsdkx/common/i2c.h>

#include <epsdkx/hal/i2c.h>
#include <stdint.h>


int i2c_init(i2c_channel_t channel, i2c_mode_e mode, uint16_t address) {
  return hal_i2c_init(channel, mode, address);
}

const i2c_pins_s *i2c_get_pins(i2c_channel_t channel) {
  return hal_i2c_get_pins(channel);
}

int i2c_putn(i2c_channel_t channel, uint8_t *tx, uint16_t n, uint16_t slave_address) {
  return hal_i2c_putn(channel, tx, n, slave_address);
}

int i2c_getn(i2c_channel_t channel, uint16_t n, uint16_t slave_address) {
  return hal_i2c_getn(channel, n, slave_address);
}

int i2c_get(i2c_channel_t channel, uint8_t *rx) {
  return hal_i2c_get(channel, rx);
}
