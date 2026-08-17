#include <epsdkx/periph/sd.h>
#include "private/sd_def.h"

#include <epsdkx/drivers/spi.h>
#include <epsdkx/common/spi.h>
#include <epsdkx/drivers/gpio.h>
#include <epsdkx/common/gpio.h>
#include <epsdkx/drivers/time.h>


#define TIMEOUT_MS 1000

typedef enum sd_command_index_e {
  SD_CMD0_INDEX,

  SD_COMMAND_COUNT
} sd_command_index_e;

/**
 * Contains hard-coded command frames that are used by this driver.
 */
static const sd_command_frame_s commands[SD_COMMAND_COUNT] = {
  [SD_CMD0_INDEX] = STATIC_CONSTRUCT_COMMAND(SD_CMD0, 0x0, 0x4A),
};

/**
 * Arranges the provided SD command and argument into a 6-byte frame,
 * defaulting CRC to 0, and returns the frame struct.
 */
static inline sd_command_frame_s sd_construct_command(sd_cmd_e cmd, uint32_t arg);

/**
 * Receives n contiguous raw bytes from the SD card.
 *
 * Returns -ETIMEDOUT if the transaction times out.
 */
static int sd_readn_raw(sd_dev_s *dev, uint8_t *rx, size_t n);

/**
 * Sends n contiguous raw bytes to the SD card.
 *
 * Returns -ETIMEDOUT if the transaction times out.
 */
static int sd_writen_raw(sd_dev_s *dev, const uint8_t *tx, size_t n);

/**
 * Sends a single SD command frame over the SPI bus.
 */
static int sd_write_command(sd_dev_s *dev, const sd_command_frame_s *tx);

/**
 * Receives an SD response of the provided type.
 */
static int sd_read_response(sd_dev_s *dev, sd_r_e type, sd_response_frame_s *rx);

/**
 * Send all initialization commands to the SD card.
 */
static int sd_init_command_sequence(sd_dev_s *dev);

int sd_init(sd_dev_s *dev, spi_channel_t channel, gpio_pin_u cs_pin) {
  int ret;

  dev->channel = channel;
  dev->cs = cs_pin;

  ret = spi_init(dev->channel, SPI_MASTER, SPI_CPOL0, SPI_CPHA0);
  gpio_write(&dev->cs, GPIO_HIGH);
  time_delay_ms(5);

  // Set frequency to 200 kHz, inside the 100-400 kHz range for initialization.
  spi_set_freq(dev->channel, 200);

  if (ret >= 0) {
    // Send 80 clock pulses to satisfy the 74 clock pulse requirement for initialization.
    for (size_t i = 0; i < 10; i++) {
      spi_read_write(dev->channel, 0);
    }
    spi_discardn(dev->channel, 10, TIMEOUT_MS);

    // Return frequency to default 2 MHz.
    spi_set_freq(dev->channel, 2000); 

    ret = sd_init_command_sequence(dev);
  } 

  return ret;
}

static inline sd_command_frame_s sd_construct_command(sd_cmd_e cmd, uint32_t arg) {
  return (sd_command_frame_s){
    .bytes = {
      0x0 << START_POS | 0x1 << TRAN_POS | cmd << CMD_POS,
      arg & 0xFF,
      (arg >> 8) & 0xFF,
      (arg >> 16) & 0xFF,
      (arg >> 24) & 0xFF,
      0x0 << CRC_POS | 0x1 << END_POS
    }
  };
}

static int sd_readn_raw(sd_dev_s *dev, uint8_t *rx, size_t n) {
  int ret;

  for (size_t i = 0; i < n; i++) {
    spi_read_write(dev->channel, 0xFF);
  }
  ret = spi_getn(dev->channel, rx, n, TIMEOUT_MS);

  return ret;
}

static int sd_writen_raw(sd_dev_s *dev, const uint8_t *tx, size_t n) {
  int ret;

  spi_readn_writen(dev->channel, (uint8_t *)tx, n);
  ret = spi_discardn(dev->channel, n, TIMEOUT_MS);

  return ret;
}

static int sd_write_command(sd_dev_s *dev, const sd_command_frame_s *tx) {
  int ret;

  ret = sd_writen_raw(dev, tx->bytes, CMD_FRAME_SIZE);

  return ret;
}

static int sd_read_response(sd_dev_s *dev, sd_r_e type, sd_response_frame_s *rx) {
  (void)dev;
  (void)type;
  (void)rx;

  return 0;
}

static int sd_init_command_sequence(sd_dev_s *dev) {
  int ret;

  gpio_write(&dev->cs, GPIO_LOW);

  // Send CMD0 to reset into SPI mode.
  ret = sd_write_command(dev, &commands[SD_CMD0_INDEX]);

  gpio_write(&dev->cs, GPIO_HIGH);

  return ret;
}
