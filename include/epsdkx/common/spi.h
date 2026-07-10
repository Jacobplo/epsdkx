#ifndef _EPSDKX_COMMON_SPI_H
#define _EPSDKX_COMMON_SPI_H

#include <epsdkx/common/gpio.h>

#include <stdint.h>


typedef uint8_t spi_channel_t;
#define SPI(n) (spi_channel_t)(n)

typedef struct spi_pins_s {
  gpio_pin_u ss;
  gpio_pin_u sclk;
  gpio_pin_u mosi;
  gpio_pin_u miso;
} spi_pins_s;

#endif
