#include <epsdkx/drivers/spi.h>

#include <epsdkx/hal/spi.h>
#include <epsdkx/common/spi.h>


int spi_init(spi_channel_t channel, spi_mode_e mode, spi_cpol_e cpol, spi_cpha_e cpha) {
  return hal_spi_init(channel, mode, cpol, cpha);
}
