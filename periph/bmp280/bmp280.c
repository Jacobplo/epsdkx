#include <epsdkx/periph/bmp280.h>

#include <epsdkx/drivers/i2c.h>
#include <epsdkx/drivers/spi.h>
#include <epsdkx/drivers/gpio.h>
#include <epsdkx/common/i2c.h>
#include <epsdkx/common/spi.h>
#include <epsdkx/common/gpio.h>
#include <epsdkx/drivers/time.h>
#include <epsdkx/generated/config.h>

#include <stdint.h>
#include <errno.h>


#if !(defined (CONFIG_I2C)) && !(defined (CONFIG_SPI))
#error Either CONFIG_I2C or CONFIG_SPI must be enabled
#endif


static void bmp280_write(bmp280_dev_s *dev, bmp280_addr_e addr, uint8_t tx);
static void bmp280_readn(bmp280_dev_s *dev, bmp280_addr_e addr, uint8_t *rx, uint8_t n);
static double bmp280_compensate_temperature(bmp280_dev_s *dev, int32_t adc_t);
static double bmp280_compensate_pressure(bmp280_dev_s *dev, int32_t adc_p);

static inline void get_compensation_params(bmp280_dev_s *dev);
static inline void configure_measurements(bmp280_dev_s *dev);

int bmp280_init_spi(bmp280_dev_s *dev, spi_channel_t channel, gpio_pin_u csb) {
  if (!CONFIG_SPI) return -EPERM;

  int ret;

  dev->type = BMP280_SPI;
  dev->channel.spi = channel;
  dev->spi_csb = csb;

  gpio_init();
  gpio_configure(&csb, GPIO_OUT);
  gpio_write(&csb, GPIO_HIGH);


  ret = spi_init(channel, SPI_MASTER, SPI_CPOL0, SPI_CPHA0);

  time_delay_ms(5);

  bmp280_write(dev, BMP280_RESET_ADDR, 0xB6);

  time_delay_ms(5);

  if (ret >= 0) {
    get_compensation_params(dev);
    configure_measurements(dev);
  }

  return ret;
}

int bmp280_init_i2c(bmp280_dev_s *dev, i2c_channel_t channel, gpio_state_e sdo_state) {
  if (!CONFIG_I2C) return -EPERM;

  int ret;

  dev->type = BMP280_I2C;
  dev->channel.i2c = channel;
  dev->i2c_addr = 0xE6 | sdo_state;

  ret = i2c_init_master(channel);

  if (ret >= 0) {
    get_compensation_params(dev);
    configure_measurements(dev);
  }

  return ret;
}

double bmp280_get_temperature(bmp280_dev_s *dev) {
  uint8_t raw[3];
  bmp280_readn(dev, BMP280_TEMP_MSB_ADDR, raw, 3);

  int32_t adc_t = (raw[0] << 12) | (raw[1] << 4) | (raw[2] >> 4);

  return bmp280_compensate_temperature(dev, adc_t);
}

double bmp280_get_pressure(bmp280_dev_s *dev) {
  uint8_t raw[6];
  bmp280_readn(dev, BMP280_PRESS_MSB_ADDR, raw, 6);

  int32_t adc_p = (raw[0] << 12) | (raw[1] << 4) | (raw[2] >> 4);
  int32_t adc_t = (raw[3] << 12) | (raw[4] << 4) | (raw[5] >> 4);

  // Call compensate_temperature() to get t_fine
  bmp280_compensate_temperature(dev, adc_t);

  return bmp280_compensate_pressure(dev, adc_p);
}

static void bmp280_write(bmp280_dev_s *dev, bmp280_addr_e addr, uint8_t tx) {
  uint8_t discard;

  uint8_t i2c_packet[2] = { addr, tx };

  switch (dev->type) {
    case BMP280_SPI:
      gpio_write(&dev->spi_csb, GPIO_LOW);

      spi_put(dev->channel.spi, addr & 0x7F);
      while (spi_get(dev->channel.spi, &discard) < 0) (void)0;

      spi_put(dev->channel.spi, tx);
      while (spi_get(dev->channel.spi, &discard) < 0) (void)0;

      gpio_write(&dev->spi_csb, GPIO_HIGH);
      break;

    case BMP280_I2C:
      i2c_putn(dev->channel.i2c, i2c_packet, 2, dev->i2c_addr);
      break;
  }
}

static void bmp280_readn(bmp280_dev_s *dev, bmp280_addr_e addr, uint8_t *rx, uint8_t n) {
  int i;
  uint8_t discard;

  switch (dev->type) {
    case BMP280_SPI:
      gpio_write(&dev->spi_csb, GPIO_LOW);

      // Select register address
      spi_put(dev->channel.spi, addr | 0x80);
      while (spi_get(dev->channel.spi, &discard) < 0) (void)0;

      // Read N bytes from sequential registers
      for (i = 0; i < n; i++) {
        spi_put(dev->channel.spi, 0);
      }

      // Get RX data
      for (i = 0; i < n; i++) {
        while (spi_get(dev->channel.spi, &rx[i]) < 0) (void)0;
      }

      gpio_write(&dev->spi_csb, GPIO_HIGH); 

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

// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value
static double bmp280_compensate_temperature(bmp280_dev_s *dev, int32_t adc_t) {
  double var1, var2, t;

  var1 = (((double)adc_t) / 16384.0 - ((double)dev->params.dig_T1) / 1024.0) *
         ((double)dev->params.dig_T2);

  var2 = ((((double)adc_t) / 131072.0 - ((double)dev->params.dig_T1) / 8192.0) *
         (((double)adc_t) / 131072.0 - ((double)dev->params.dig_T1) / 8192.0)) *
         ((double)dev->params.dig_T3);

  dev->t_fine = (int32_t)(var1 + var2);
  t = (var1 + var2) / 5120.0;
  return t;
}

// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
static double bmp280_compensate_pressure(bmp280_dev_s *dev, int32_t adc_p) {
  double var1, var2, p;
  var1 = ((double)dev->t_fine / 2.0) - 64000.0;

  var2 = var1 * var1 * ((double)dev->params.dig_P6) / 32768.0;
  var2 = var2 + var1 * ((double)dev->params.dig_P5) * 2.0;
  var2 = (var2 / 4.0) + (((double)dev->params.dig_P4) * 65536.0);

  var1 = (((double)dev->params.dig_P3) * var1 * var1 / 524288.0 + ((double)dev->params.dig_P2) * var1) /
         524288.0;
  var1 = (1.0 + var1 / 32768.0) * ((double)dev->params.dig_P1);

  // Avoid exception caused by division by zero
  if (var1 == 0.0) {
    return 0;
  }

  p = 1048576.0 - (double)adc_p;
  p = (p - (var2 / 4096.0)) * 6250.0 / var1;

  var1 = ((double)dev->params.dig_P9) * p * p / 2147483648.0;
  var2 = p * ((double)dev->params.dig_P8) / 32768.0;

  p = p + (var1 + var2 + ((double)dev->params.dig_P7)) / 16.0;
  return p;
}

static inline void get_compensation_params(bmp280_dev_s *dev) {
  bmp280_readn(dev, BMP280_CALIB_BASE_ADDR, (uint8_t *)&dev->params, sizeof(bmp280_compensation_params_s));
}

static inline void configure_measurements(bmp280_dev_s *dev) {
  // Set 62.5 ms standby, IIR filter coefficient of 16, and 4-wire spi mode
  bmp280_write(dev, BMP280_CONFIG_ADDR, 0x3C);

  // Set 1x temperature oversampling, 4x pressure oversampling, and normal mode
  bmp280_write(dev, BMP280_CTRL_MEAS_ADDR, 0x2F);
}
