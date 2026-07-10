#include <epsdkx/drivers/spi.h>

#include <epsdkx/hal/spi.h>
#include <epsdkx/common/spi.h>


int spi_init(spi_channel_t channel) {
  return hal_spi_init(channel);
}
