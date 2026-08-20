#include <epsdkx/drivers/i2c.h>

#include <epsdkx/common/i2c.h>
#include <epsdkx/hal/i2c.h>
#include <epsdkx/drivers/time.h>

#include <stdint.h>
#include <stddef.h>
#include <errno.h>


int i2c_init(i2c_channel_t channel, i2c_mode_e mode, uint16_t address) {
  return hal_i2c_init(channel, mode, address);
}

const i2c_pins_s *i2c_get_pins(i2c_channel_t channel) {
  return hal_i2c_get_pins(channel);
}

bool i2c_is_busy(i2c_channel_t channel) {
  return hal_i2c_is_busy(channel);
}

int i2c_writen(i2c_channel_t channel, uint8_t *tx, uint16_t n, uint16_t slave_address) {
  return hal_i2c_writen(channel, tx, n, slave_address);
}

int i2c_readn(i2c_channel_t channel, uint16_t n, uint16_t slave_address) {
  return hal_i2c_readn(channel, n, slave_address);
}

int i2c_get(i2c_channel_t channel, uint8_t *rx) {
  return hal_i2c_get(channel, rx);
}

int i2c_getn(i2c_channel_t channel, uint8_t *rx, size_t n, uint32_t timeout_ms) {
  int ret = 0;

  uint32_t start = time_get_ticks();  

  for (size_t i = 0; i < n; i++) {
    while (i2c_get(channel, &rx[i]) < 0) {
      uint32_t cur = time_get_ticks();
      if (time_ticks_to_ms(cur - start) > timeout_ms) {
        ret = -ETIMEDOUT;
        goto getn_return;
      }
    }
  }

getn_return:
  return ret;
}

int i2c_write(i2c_channel_t channel, uint8_t *tx, uint16_t slave_address) {
  return hal_i2c_writen(channel, tx, 1, slave_address);
}

int i2c_read(i2c_channel_t channel, uint16_t slave_address) {
  return hal_i2c_readn(channel, 1, slave_address);
}
