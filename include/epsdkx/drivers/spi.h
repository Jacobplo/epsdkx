/**
 * Public API for the SPI interface driver.
 * 
 * Unless otherwise stated, all functions that return int return a negative
 * errno on error, and 0 on success.
 */

#ifndef _EPSDKX_SPI_H
#define _EPSDKX_SPI_H

#include <epsdkx/common/spi.h>

#include <stdint.h>
#include <stddef.h>


/**
 * Generic SPI interface intialization that works for either master or slave
 * mode.
 *
 * In master mode: defaults to a frequency in the 1-2 MHz range.
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
 * Retrieves n byte of data from the internal RX buffer into rx.
 *
 * Unlike spi_get(), this will wait for each byte to arrive, so a timeout is
 * used.
 *
 * Returns -ETIMEOUT on timeout.
 */
int spi_getn(spi_channel_t channel, uint8_t *rx, size_t n, uint32_t timeout_ms);

/**
 * Returns the GPIO pins used by the SPI interface.
 */
const spi_pins_s *spi_get_pins(spi_channel_t channel);

/**
 * Sets the master mode as close as possible to the provided frequency, in kHz.
 * No guarantee can be made that the clock will able to be set exactly as
 * configured, but in most cases it should be reasonably accurate.
 *
 * Has no effect in slave mode.
 */
int spi_set_freq(spi_channel_t channel, uint32_t freq_khz);

/**
 * Discards n bytes from the internal RX buffer for cases where data is sent,
 * but none should be received.
 *
 * A timeout is used, because this will wait for each byte to arrive.
 *
 * Returns -ETIMEOUT on timeout.
 */
int spi_discardn(spi_channel_t channel, size_t n, uint32_t timeout_ms);

#endif
