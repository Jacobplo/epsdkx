#include <epsdkx/periph/bmp280.h>

#include <epsdkx/drivers/i2c.h>
#include <epsdkx/drivers/spi.h>
#include <epsdkx/drivers/gpio.h>
#include <epsdkx/common/i2c.h>
#include <epsdkx/common/spi.h>
#include <epsdkx/common/gpio.h>

#include <stdint.h>


int bmp280_init_spi(bmp280_dev_s *dev, spi_channel_t channel, gpio_pin_u csb) {
  dev->type = BMP280_SPI;
  dev->channel.spi = channel;
  dev->spi_csb = csb;

  gpio_init();
  gpio_configure(&csb, GPIO_OUT);
  gpio_write(&csb, GPIO_HIGH);

  return spi_init(channel, SPI_MASTER, SPI_CPOL0, SPI_CPHA0); 
}

int bmp280_init_i2c(bmp280_dev_s *dev, i2c_channel_t channel, gpio_state_e sdo_state) {
  dev->type = BMP280_I2C;
  dev->channel.i2c = channel;
  dev->i2c_addr = 0xE6 | sdo_state;

  return i2c_init_master(channel);
}

void bmp280_write(bmp280_dev_s *dev, bmp280_addr_e addr, uint8_t tx) {
  uint8_t i2c_packet[2] = { addr, tx };

  switch (dev->type) {
    case BMP280_SPI:
      gpio_write(&dev->spi_csb, GPIO_LOW);
      spi_put(dev->channel.spi, addr);
      spi_put(dev->channel.spi, tx);
      gpio_write(&dev->spi_csb, GPIO_HIGH);
      break;

    case BMP280_I2C:
      i2c_putn(dev->channel.i2c, i2c_packet, 2, dev->i2c_addr);
      break;
  }
}

void bmp280_readn(bmp280_dev_s *dev, bmp280_addr_e addr, uint8_t *rx, uint8_t n) {
  int i;

  switch (dev->type) {
    case BMP280_SPI:
      gpio_write(&dev->spi_csb, GPIO_LOW);

      // Select register address
      spi_put(dev->channel.spi, addr | 0x80);

      // Read N bytes from sequential registers
      for (i = 0; i < n; i++) {
        spi_put(dev->channel.spi, 0);
      }

      gpio_write(&dev->spi_csb, GPIO_HIGH); 

      // Get RX data
      for (i = 0; i < n; i++) {
        while (spi_get(dev->channel.spi, &rx[i]) < 0) (void)0;
      }

      break;

    case BMP280_I2C:
      // Select register address
      i2c_putn(dev->channel.i2c, (uint8_t *)&addr, 1, dev->i2c_addr);

      // Read N bytes from sequential registers
      i2c_getn(dev->channel.i2c, n, dev->i2c_addr);

      // Get RX data
      for (i = 0; i < n; i++) {
        while (i2c_get(dev->channel.i2c, &rx[i]) < 0) (void)0;
      }
      break;
  }
}
