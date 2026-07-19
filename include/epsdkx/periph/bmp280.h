#ifndef _EPSDKX_PERIPH_BMP280_H
#define _EPSDKX_PERIPH_BMP280_H

#include <epsdkx/common/i2c.h>
#include <epsdkx/common/spi.h>


typedef enum bmp280_type_e {
  BMP280_I2C,
  BMP280_SPI
} bmp280_type_e;

typedef struct bmp280_dev_s {
  bmp280_type_e type;
  union {
    i2c_channel_t i2c;
    spi_channel_t spi;
  } channel;
  gpio_pin_u spi_csb;
  uint8_t i2c_addr;
} bmp280_dev_s;

typedef enum bmp280_addr_e {
  BMP280_ID_ADDR         = 0xD0,
  BMP280_RESET_ADDR      = 0xE0,
  BMP280_STATUS_ADDR     = 0xF3,
  BMP280_CTRL_MEAS_ADDR  = 0xF4,
  BMP280_CONFIG_ADDR     = 0xF5,
  BMP280_PRESS_MSB_ADDR  = 0xF7,
  BMP280_PRESS_LSB_ADDR  = 0xF8,
  BMP280_PRESS_XLSB_ADDR = 0xF9,
  BMP280_TEMP_MSB_ADDR   = 0xFA,
  BMP280_TEMP_LSB_ADDR   = 0xFB,
  BMP280_TEMP_XLSB_ADDR  = 0xFC
} bmp280_addr_e;


int bmp280_init_spi(bmp280_dev_s *dev, spi_channel_t channel, gpio_pin_u csb);

int bmp280_init_i2c(bmp280_dev_s *dev, i2c_channel_t channel, gpio_state_e sdo_state);

void bmp280_write(bmp280_dev_s *dev, bmp280_addr_e addr, uint8_t tx);

void bmp280_readn(bmp280_dev_s *dev, bmp280_addr_e addr, uint8_t *rx, uint8_t n);

#endif
