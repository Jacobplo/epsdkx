#ifndef _EPSDKX_SPI_H
#define _EPSDKX_SPI_H

#include <epsdkx/common/spi.h>

#include <stdint.h>
#include <stddef.h>


int spi_init(spi_channel_t channel, spi_mode_e mode, spi_cpol_e cpol, spi_cpha_e cpha);

const spi_pins_s *spi_get_pins(spi_channel_t channel);

int spi_read_write(spi_channel_t channel, uint8_t tx);

int spi_get(spi_channel_t channel, uint8_t *rx);

int spi_readn_writen(spi_channel_t channel, uint8_t *tx, size_t n);



#endif
