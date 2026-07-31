/**
 * Public API for the SPI interface driver.
 *
 * Unless otherwise stated, all functions return a negative errno on error,
 * and 0 on success.
 */

#ifndef _EPSDKX_SPI_H
#define _EPSDKX_SPI_H

#include <epsdkx/common/spi.h>

#include <stdint.h>
#include <stddef.h>


/**
 * Generic SPI interface intialization that works for either master or slave
 * mode.
 */
int spi_init(spi_channel_t channel, spi_mode_e mode, spi_cpol_e cpol, spi_cpha_e cpha);

/**
 * Transmits a single byte of data and receives a single byte of data into
 * the internal RX buffer.
 */
int spi_read_write(spi_channel_t channel, uint8_t tx);

/**
 * Transmits a contiguous chunk of n bytes of data and receives n bytes of data
 * into the internal RX buffer.
 */
int spi_readn_writen(spi_channel_t channel, uint8_t *tx, size_t n);

/**
 * Retrieves a single byte of data from the internal RX buffer into rx.
 */
int spi_get(spi_channel_t channel, uint8_t *rx);

/**
 * Returns the GPIO pins used by the SPI interface.
 */
const spi_pins_s *spi_get_pins(spi_channel_t channel);

#endif
