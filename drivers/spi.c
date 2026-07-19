#include <epsdkx/drivers/spi.h>

#include <epsdkx/hal/spi.h>
#include <epsdkx/common/spi.h>


int spi_init(spi_channel_t channel, spi_mode_e mode, spi_cpol_e cpol, spi_cpha_e cpha) {
  return hal_spi_init(channel, mode, cpol, cpha);
}

const spi_pins_s *spi_get_pins(spi_channel_t channel) {
  return hal_spi_get_pins(channel);
}

int spi_put(spi_channel_t channel, uint8_t tx) {
  return hal_spi_put(channel, tx);
}

int spi_get(spi_channel_t channel, uint8_t *rx) {
  return hal_spi_get(channel, rx);
}

