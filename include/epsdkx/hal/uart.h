/**
* Internal contract for HAL UART interface implementation
*
* Is it the responsibilty of the implementation to provide a way to map
* an input uart_channel_t to the interface's hardware. All methods that
* accept a 'channel' argument should perform validation to confirm that
* the interface is valid.
*
* No function is provided for receiving UART data because this should be
* handled using interrupts. Data should be received into the internal RX
* buffer.
*
* Unless otherwise stated, all functions that return int should return a
* negative errno on error, and 0 on success.
*/

#ifndef _EPSDKX_HAL_UART_H
#define _EPSDKX_HAL_UART_H

#include <epsdkx/common/uart.h>

#include <stdint.h>


/**
 * Should initialize the given UART channel only, with the provided baud rate.
 *
 * Should use an 8-bit word length, with 1 stop bit and 0 parity bits.
 */
int hal_uart_init(uart_channel_t channel, uint32_t baud_rate);

/**
 * Should send a byte of data.
 */
int hal_uart_write(uart_channel_t channel, uint8_t tx);

/**
 * Should return the value returned by hal_rx_buffer_get() on the internal
 * RX buffer.
 */
int hal_uart_get(uart_channel_t channel, uint8_t *rx);

/**
 * Should returns the GPIO pins associated with the interface, which should be
 * available in the internal interface mapping.
 */
const uart_pins_s *hal_uart_get_pins(uart_channel_t channel);

#endif
