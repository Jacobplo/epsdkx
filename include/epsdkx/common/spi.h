#ifndef _EPSDKX_COMMON_SPI_H
#define _EPSDKX_COMMON_SPI_H

#include <epsdkx/common/gpio.h>

#include <stdint.h>


typedef uint8_t spi_channel_t;
#define SPI(n) (spi_channel_t)(n)

typedef struct spi_pins_s {
  gpio_pin_u nss;
  gpio_pin_u sclk;
  gpio_pin_u mosi;
  gpio_pin_u miso;
} spi_pins_s;

typedef enum spi_mode_e {
  SPI_MASTER,
  SPI_SLAVE
} spi_mode_e;

typedef enum spi_cpol_e {
  SPI_CPOL0,
  SPI_CPOL1
} spi_cpol_e;

typedef enum spi_cpha_e {
  SPI_CPHA0,
  SPI_CPHA1
} spi_cpha_e;

#endif
