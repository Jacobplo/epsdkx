#include <epsdkx/periph/sd.h>

#include <epsdkx/drivers/spi.h>
#include <epsdkx/common/spi.h>


int sd_init(sd_dev_s *dev, spi_channel_t channel) {
  (void)dev;
  (void)channel;

  return 0;
}
