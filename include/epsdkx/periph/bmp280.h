#ifndef _EPSDKX_PERIPH_BMP280_H
#define _EPSDKX_PERIPH_BMP280_H

#include <epsdkx/common/i2c.h>
#include <epsdkx/common/spi.h>


typedef enum bmp280_type_e {
  BMP280_I2C,
  BMP280_SPI
} bmp280_type_e;

typedef struct bmp280_compensation_params_s {
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;
  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;
  uint8_t dig_H1;
  int8_t dig_H2;
  uint8_t dig_H3;
  int16_t dig_H4;
  int16_t dig_H5;
  int8_t dig_H6;
} __attribute__((packed)) bmp280_compensation_params_s;

typedef struct bmp280_dev_s {
  bmp280_type_e type;
  uint8_t id;
  union {
    i2c_channel_t i2c;
    spi_channel_t spi;
  } channel;
  gpio_pin_u spi_csb;
  uint8_t i2c_addr;

  bmp280_compensation_params_s params;
  int32_t t_fine;
} bmp280_dev_s;

typedef enum bmp280_addr_e {
  BMP280_CALIB1_BASE_ADDR = 0x88,
  BMP280_CALIB2_BASE_ADDR = 0xE1,

  BMP280_ID_ADDR         = 0xD0,
  BMP280_RESET_ADDR      = 0xE0,

  BMP280_CTRL_HUM        = 0xF2,
  BMP280_STATUS_ADDR     = 0xF3,
  BMP280_CTRL_MEAS_ADDR  = 0xF4,
  BMP280_CONFIG_ADDR     = 0xF5,
  BMP280_PRESS_MSB_ADDR  = 0xF7,
  BMP280_PRESS_LSB_ADDR  = 0xF8,
  BMP280_PRESS_XLSB_ADDR = 0xF9,
  BMP280_TEMP_MSB_ADDR   = 0xFA,
  BMP280_TEMP_LSB_ADDR   = 0xFB,
  BMP280_TEMP_XLSB_ADDR  = 0xFC,
  BMP280_HUM_MSB_ADDR    = 0xFD,
  BMP280_HUM_LSB_ADDR    = 0xFE
} bmp280_addr_e;


int bmp280_init_spi(bmp280_dev_s *dev, spi_channel_t channel, gpio_pin_u csb);

int bmp280_init_i2c(bmp280_dev_s *dev, i2c_channel_t channel, gpio_state_e sdo_state);

// Returns temperature data in degrees Celsius
double bmp280_get_temperature(bmp280_dev_s *dev);

// Returns pressure data in Pascals
double bmp280_get_pressure(bmp280_dev_s *dev);

// Returns pressure data in percentage relative humidity
double bmp280_get_humidity(bmp280_dev_s *dev);

#endif
