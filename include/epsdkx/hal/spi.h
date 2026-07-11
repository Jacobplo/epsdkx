#ifndef _EPSDKX_HAL_SPI_H
#define _EPSDKX_HAL_SPI_H

#include <epsdkx/common/spi.h>

#include <stdint.h>


int hal_spi_init(spi_channel_t channel, spi_mode_e mode, spi_cpol_e cpol, spi_cpha_e cpha);

const spi_pins_s *hal_spi_get_pins(spi_channel_t channel);

void hal_spi_put(spi_channel_t channel, uint8_t tx);

int hal_spi_get(spi_channel_t channel, uint8_t *rx);


#endif
