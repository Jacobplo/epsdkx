/**
 * Public API for the UART interface driver.
 *
 * Received UART data is asynchronously put into the internal RX buffer,
 * and can be retrieved with uart_get().
 *
 * Unless otherwise stated, all functions that return int return a negative
 * errno on error, and 0 on success.
 */

#ifndef _EPSDKX_UART_H
#define _EPSDKX_UART_H

#include <epsdkx/common/uart.h>

#include <stdint.h>
#include <stddef.h>

/**
 * Initializes a UART interface with the given baud rate.
 */
int uart_init(uart_channel_t channel, uint32_t baud_rate);

/**
 * Transmits a single byte of data.
 */
int uart_write(uart_channel_t channel, uint8_t tx);

/**
 * Transmits a contiguous chunk of n bytes of data.
 */
int uart_writen(uart_channel_t channel, const uint8_t *tx, size_t n);

/**
 * Retrieves a single byte of data from the internal RX buffer into rx.
 */
int uart_get(uart_channel_t channel, uint8_t *rx);

/**
 * Returns the GPIO pins used by the UART interface.
 */
const uart_pins_s *uart_get_pins(uart_channel_t channel);

#endif
