/**
 * Public API driver for SD card SPI mode.
 *
 * Unless otherwise stated, all functions that return int return a
 * negative errno on error, and 0 on success.
 */

#ifndef _EPSDKX_PERIPH_SD_H
#define _EPSDKX_PERIPH_SD_H

#include <epsdkx/common/spi.h>
#include <epsdkx/common/gpio.h>


/**
 * Used to store information about a specific SD card / module in user-space.
 */
typedef struct sd_dev_s {
  spi_channel_t channel;
  gpio_pin_u cs;
} sd_dev_s;

/**
 * Initializes the provided SPI channel for use with the SD card and
 * sends initialization commands to the card for use with this driver.
 *
 * dev should be declared empty by the user. It stores data about the device
 * and interface.
 */
int sd_init(sd_dev_s *dev, spi_channel_t channel, gpio_pin_u cs_pin);

#endif
