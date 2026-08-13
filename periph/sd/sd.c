#include <epsdkx/periph/sd.h>

#include <epsdkx/drivers/spi.h>
#include <epsdkx/common/spi.h>
#include <epsdkx/drivers/gpio.h>
#include <epsdkx/common/gpio.h>


int sd_init(sd_dev_s *dev, spi_channel_t channel, gpio_pin_u cs_pin) {
  (void)dev;
  (void)channel;
  (void)cs_pin;

  return 0;
}
