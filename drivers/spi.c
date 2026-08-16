#include <epsdkx/drivers/spi.h>

#include <epsdkx/hal/spi.h>
#include <epsdkx/common/spi.h>
#include <epsdkx/drivers/time.h>

#include <stdint.h>
#include <stddef.h>
#include <errno.h>


int spi_init(spi_channel_t channel, spi_mode_e mode, spi_cpol_e cpol, spi_cpha_e cpha) {
  return hal_spi_init(channel, mode, cpol, cpha);
}

const spi_pins_s *spi_get_pins(spi_channel_t channel) {
  return hal_spi_get_pins(channel);
}

int spi_read_write(spi_channel_t channel, uint8_t tx) {
  return hal_spi_read_write(channel, tx);
}

int spi_get(spi_channel_t channel, uint8_t *rx) {
  return hal_spi_get(channel, rx);
}

int spi_getn(spi_channel_t channel, uint8_t *rx, size_t n, uint32_t timeout_ms) {
  int ret = 0;

  uint32_t start = time_get_ticks();  

  for (size_t i = 0; i < n; i++) {
    while (spi_get(channel, &rx[i]) < 0) {
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

int spi_readn_writen(spi_channel_t channel, uint8_t *tx, size_t n) {
  int ret;

  for (size_t i = 0; i < n; i++) { 
    if ((ret = hal_spi_read_write(channel, tx[i])) < 0) {
      return ret;
    };
  }

  return 0;
}

int spi_set_freq(spi_channel_t channel, uint32_t freq_khz) {
  return hal_spi_set_freq(channel, freq_khz);
}

int spi_discardn(spi_channel_t channel, size_t n, uint32_t timeout_ms) {
  int ret = 0;
  uint8_t discard;

  uint32_t start = time_get_ticks();  

  for (size_t i = 0; i < n; i++) {
    while (spi_get(channel, &discard) < 0) {
      uint32_t cur = time_get_ticks();
      if (time_ticks_to_ms(cur - start) > timeout_ms) {
        ret = -ETIMEDOUT;
        goto discardn_return;
      };
    };
  }

  discardn_return:
  return ret;
}
