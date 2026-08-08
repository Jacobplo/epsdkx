/**
 * Public API driver for the u-blox Neo-6 GPS receiver.
 *
 * Unless otherwise stated, all functions that return int should return a
 * negative errno on error, and 0 on success.
 */

#ifndef _EPSDKX_PERIPH_NEO6_H
#define _EPSDKX_PERIPH_NEO6_H

#include <epsdkx/common/uart.h>


typedef struct neo6_dev_s {
  uart_channel_t channel;
} neo6_dev_s;

/**
 * Initializes the provided UART channel for use with the GPS receiver and
 * sends configuration messages to the device for use with this driver.
 *
 * NOTE: Configuration made by these messages will persist in the device
 * read-only memory, and can be reset to defaults with neo6_reset().
 */
int neo6_init(neo6_dev_s *dev, uart_channel_t channel);


/**
 * Resets the GPS receiver to its default factory configuration.
 */
int neo6_reset(uart_channel_t channel);

#endif
