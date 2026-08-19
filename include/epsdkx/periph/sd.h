/**
 * Public API driver for SD card SPI mode.
 *
 * Initializes with a block length of 512 bytes.
 * 512 byte buffers must be used for single block read and write operations.
 * 512 * n byte buffers must be used for multiple block read and write operations.
 *
 * Unless otherwise stated, all functions that return int return a
 * negative errno on error, and 0 on success.
 */

#ifndef _EPSDKX_PERIPH_SD_H
#define _EPSDKX_PERIPH_SD_H

#include <epsdkx/common/spi.h>
#include <epsdkx/common/gpio.h>

#include <stdint.h>
#include <stddef.h>

/**
 * SD card version number
 */
typedef enum sd_version_e {
  SD_VER_1X,
  SD_VER_2X,
} sd_version_e;

/**
* SD card capacity class
*/
typedef enum sd_capacity_e {
  SD_CAPACITY_STANDARD,
  SD_CAPACITY_HIGH,
} sd_capacity_e;

/**
 * Used to store information about a specific SD card / module in user-space.
 */
typedef struct sd_dev_s {
  spi_channel_t channel;
  gpio_pin_u cs;
  sd_version_e version;
  sd_capacity_e capacity_class;
  struct {
    uint16_t read_bl_len;
    uint16_t c_size;
    uint8_t c_size_mult;
    uint8_t sector_size;
  } csd;
  struct {
    uint64_t capacity_bytes; 
    uint16_t sector_size;
    uint32_t sector_count;
  } prop;
} sd_dev_s;

/**
 * Initializes the provided SPI channel for use with the SD card and
 * sends initialization commands to the card for use with this driver.
 *
 * dev should be declared empty by the user. It stores data about the device
 * and interface.
 */
int sd_init(sd_dev_s *dev, spi_channel_t channel, gpio_pin_u cs_pin);

/**
 * Reads a 512 byte block from the SD card at the provided sector address.
 */
int sd_read_block(sd_dev_s *dev, uint32_t sector, uint8_t *buf);

/**
 * Writes a 512 byte block to the SD card at the provided sector address.
 */
int sd_write_block(sd_dev_s *dev, uint32_t sector, const uint8_t *buf);

/**
 * Reads n contiguous 512 byte blocks from the SD card starting from the
 * provided sector address.
 */
int sd_readn_block(sd_dev_s *dev, uint32_t sector, uint8_t *buf, size_t n);

/**
 * Writes n contiguous 512 byte blocks to the SD card starting from the
 * provided sector address.
 */
int sd_writen_block(sd_dev_s *dev, uint32_t sector, const uint8_t *buf, size_t n);

#endif
