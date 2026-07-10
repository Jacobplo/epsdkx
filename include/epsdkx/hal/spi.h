#ifndef _EPSDKX_HAL_SPI_H
#define _EPSDKX_HAL_SPI_H

#include <epsdkx/hal/gpio.h>

#include <stdint.h>


typedef uint8_t hal_spi_channel_t;
#define SPI(n) (hal_spi_channel_t)(n)

typedef struct hal_spi_pins_s {
  hal_gpio_pin_u ss;
  hal_gpio_pin_u sclk;
  hal_gpio_pin_u mosi;
  hal_gpio_pin_u miso;
} hal_spi_pins_s;

int hal_spi_init(hal_spi_channel_t channel);

#endif
