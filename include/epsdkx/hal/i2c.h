/**
* Internal contract for HAL I2C interface implementation
*
* Is it the responsibilty of the implementation to provide a way to map
* an input i2c_channel_t to the interface's hardware. All methods that
* accept a 'channel' argument should perform validation to confirm that
* the interface is valid.
*
* All functions should be able to perform their respective operations
* on interfaces in either slave or master mode, so the mode should be
* part of the internal mapping.
*
* Unless otherwise stated, all functions that return int should return a
* negative errno on error, and 0 on success.
*/

#ifndef _EPSDKX_HAL_I2C_H
#define _EPSDKX_HAL_I2C_H

#include <epsdkx/common/i2c.h>

#include <stdbool.h>
#include <stdint.h>


/**
 * Should initialize the given I2C channel only, with the provided mode,
 * and if in slave mode, with the provided address.
 *
 * In master mode: Should be configured in fast mode at 400 kHz. To ensure that
 * this can be reasonably achieved, it can be expected that the each board
 * provides timing parameters in its Kconfig file. For example, 
 * CONFIG_STM32F1_I2C_CCR contains the value of the I2C CCR for STM32F1 targets,
 * as found on the board datasheet.
 *
 * In slave mode: the provided address should be a 7-bit address.
 *
 * It is expected that this function be called before any other i2c function
 * at the user-level.
 */
int hal_i2c_init(i2c_channel_t channel, i2c_mode_e mode, uint16_t address);

/**
 * Should send n contiguous data packets (bytes) in a single transmission.
 * This means that n bytes must be sent between a single START and STOP signal.
 *
 * This function may or may not be implemented using interrupts.
 */
int hal_i2c_writen(i2c_channel_t channel, uint8_t *tx, uint16_t n, uint16_t slave_address);

/**
 * Should receive n contiguous data packets (bytes) in a single transmission.
 * This means that n bytes must be received between a single START and STOP 
 * signal.
 *
 * This function may or may not be implemented using interrupts, but must always
 * read the data into the internal RX buffer to ensure consistency for the 
 * public API.
 */
int hal_i2c_readn(i2c_channel_t channel, uint16_t n, uint16_t slave_address);

/**
 * Should return the value returned by hal_rx_buffer_get() on the internal
 * RX buffer.
 */
int hal_i2c_get(i2c_channel_t channel, uint8_t *rx);

/**
 * Should returns the GPIO pins associated with the interface, which should be
 * available in the internal interface mapping.
 */
const i2c_pins_s *hal_i2c_get_pins(i2c_channel_t channel);

/**
 * Should return true if the bus the interface is on is still being used for
 * I2C transmision. Otherwise, return false.
 */
bool hal_i2c_is_busy(i2c_channel_t channel);

#endif
