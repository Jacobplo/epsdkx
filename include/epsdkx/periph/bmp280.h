#ifndef _EPSDKX_PERIPH_BMP280_H
#define _EPSDKX_PERIPH_BMP280_H


typedef enum bmp280_type_e {
  BMP280_I2C,
  BMP280_SPI
} bmp280_type_e;

typedef struct bmp280_dev_s {
  bmp280_type_e type;
} bmp280_dev_s;

#endif
