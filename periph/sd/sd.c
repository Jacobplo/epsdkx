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
 * defaulting CRC to 0.
 */
static inline sd_command_frame_s sd_construct_command(sd_cmd_e cmd, uint32_t arg);

/**
 * Sends a single SD command frame over the SPI bus.
 */
static int sd_write_command(sd_dev_s *dev, const sd_command_frame_s *tx);

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

    // Send CMD0 to reset into SPI mode.
    sd_write_command(dev, &commands[SD_CMD0_INDEX]);
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

/**
 * Sends a single SD command frame over the SPI bus.
 */
static int sd_write_command(sd_dev_s *dev, const sd_command_frame_s *tx) {
  int ret;

  gpio_write(&dev->cs, GPIO_LOW);

  spi_readn_writen(dev->channel, (uint8_t *)tx->bytes, FRAME_SIZE);
  ret = spi_discardn(dev->channel, FRAME_SIZE, TIMEOUT_MS);

  gpio_write(&dev->cs, GPIO_HIGH);

  return ret;
}
