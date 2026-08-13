/**
* Internal contract for HAL SPI interface implementation
*
* Is it the responsibilty of the implementation to provide a way to map
* an input spi_channel_t to the interface's hardware. All methods that
* accept a 'channel' argument should perform validation to confirm that
* the interface is valid.
*
* All functions should be able to perform their respective operations
* on interfaces in either slave or master mode.
*
* Unless otherwise stated, all functions that return int should return a
* negative errno on error, and 0 on success.
*/

#ifndef _EPSDKX_HAL_SPI_H
#define _EPSDKX_HAL_SPI_H

#include <epsdkx/common/spi.h>

#include <stdint.h>

/**
 * Should initialize the given SPI channel only, with the provided mode
 * and clock mode.
 *
 * The interface should be configured in MSB first mode, with a data width of
 * 8-bits.
 *
 * In master mode: by default, the clock should be configured with a frequency within
 * the 1-2 MHz range, or as close to that range as possible. The interface
 * should not be configured with slave select pins, because those are
 * expected to be the responsibility of user-level code.
 *
 * It is expected that this function be called before any other spi function
 * at the user-level.
 */
int hal_spi_init(spi_channel_t channel, spi_mode_e mode, spi_cpol_e cpol, spi_cpha_e cpha);

/**
 * Should send a byte of data and receive a byte of data into the internal
 * RX buffer.
 *
 * This function may or may not be implemented using interrupts, but must always
 * read the data into the internal RX buffer to ensure consistency for the 
 * public API.
 */
int hal_spi_read_write(spi_channel_t channel, uint8_t tx);

/**
 * Should return the value returned by hal_rx_buffer_get() on the internal
 * RX buffer.
 */
int hal_spi_get(spi_channel_t channel, uint8_t *rx);

/**
 * Should returns the GPIO pins associated with the interface, which should be
 * available in the internal interface mapping.
 */
const spi_pins_s *hal_spi_get_pins(spi_channel_t channel);

/**
 * Should set the master mode clock frequency to the provided frequency, in kHz.
 * If it is not possible to set the clock to the provided frequency, select a
 * frequency as close as possible to the provided frequency.
 *
 * Should have no effect in slave mode.
 */
int hal_spi_set_freq(spi_channel_t channel, uint32_t freq_khz);

#endif
