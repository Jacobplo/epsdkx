/**
 * Public API driver for the AT6558 GPS receiver.
 *
 * Unless otherwise stated, all functions that return int should return a
 * negative errno on error, and 0 on success.
 */

#ifndef _EPSDKX_PERIPH_AT6558_H
#define _EPSDKX_PERIPH_AT6558_H

#include <epsdkx/common/uart.h>


#define AT6558_FRAME_SIZE 128

typedef struct at6558_dev_s {
  uart_channel_t channel;
  uint8_t frame[AT6558_FRAME_SIZE];
} at6558_dev_s;

/**
 * Initializes the provided UART channel for use with the GPS receiver and
 * sends configuration messages to the device for use with this driver.
 */
int at6558_init(at6558_dev_s *dev, uart_channel_t channel);

#endif
