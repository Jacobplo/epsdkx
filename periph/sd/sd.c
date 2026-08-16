#include <epsdkx/periph/sd.h>

#include <epsdkx/drivers/spi.h>
#include <epsdkx/common/spi.h>
#include <epsdkx/drivers/gpio.h>
#include <epsdkx/common/gpio.h>
#include <epsdkx/drivers/time.h>


#define TIMEOUT_MS 1000


int sd_init(sd_dev_s *dev, spi_channel_t channel, gpio_pin_u cs_pin) {
  int ret;

  dev->channel = channel;
  dev->cs = cs_pin;

  ret = spi_init(dev->channel, SPI_MASTER, SPI_CPOL0, SPI_CPHA0);
  gpio_write(&dev->cs, GPIO_HIGH);
  time_delay_ms(5);

  // Set frequency to 200 kHz, inside the 100-400 kHz range for initialization.
  spi_set_freq(dev->channel, 200);

  if (ret >= 0) {
    // Send 80 clock pulses to satisfy the 74 clock pulse requirement for initialization.
    for (size_t i = 0; i < 10; i++) {
      spi_read_write(dev->channel, 0);
    }
    spi_discardn(dev->channel, 10, TIMEOUT_MS);

    // Return frequency to default 2 MHz.
    spi_set_freq(dev->channel, 2000);
  } 

  return ret;
}
