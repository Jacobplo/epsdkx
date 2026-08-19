#include <epsdkx/periph/sd.h>
#include "private/sd_def.h"

#include <epsdkx/drivers/spi.h>
#include <epsdkx/common/spi.h>
#include <epsdkx/drivers/gpio.h>
#include <epsdkx/common/gpio.h>
#include <epsdkx/drivers/time.h>
#include <epsdkx/generated/config.h>

#include <stdint.h>
#include <errno.h>


#if !defined (CONFIG_SPI)
#error CONFIG_SPI must be enabled to use the SD driver
#endif

#if CONFIG_RX_BUFFER_SIZE < 528
#error CONFIG_RX_BUFFER_SIZE must be at least 528 to use the SD driver
#endif


#define TIMEOUT_MS 1000

typedef enum sd_command_index_e {
  SD_CMD0_INDEX,
  SD_CMD8_INDEX,
  SD_CMD55_INDEX,
  SD_ACMD41_INDEX,
  SD_CMD58_INDEX,
  SD_CMD16_INDEX,
  SD_CMD9_INDEX,

  SD_COMMAND_COUNT
} sd_command_index_e;

/**
 * Contains hard-coded command frames that are used by this driver.
 */
static const sd_command_frame_s commands[SD_COMMAND_COUNT] = {
  [SD_CMD0_INDEX]   = STATIC_CONSTRUCT_COMMAND(SD_CMD0,   0x00,      0x4A), ///< No argument, CRC required
  [SD_CMD8_INDEX]   = STATIC_CONSTRUCT_COMMAND(SD_CMD8,   0x100,     0x6A), ///< Voltage range and check pattern, CRC required
  [SD_CMD55_INDEX]  = STATIC_CONSTRUCT_COMMAND(SD_CMD55,  0x00,      0x00), ///< No argument, no CRC
  [SD_ACMD41_INDEX] = STATIC_CONSTRUCT_COMMAND(SD_ACMD41, 0x1 << 30, 0x00), ///< Host high capacity support, no CRC
  [SD_CMD58_INDEX]  = STATIC_CONSTRUCT_COMMAND(SD_CMD58,  0x00,      0x00), ///< No argument, no CRC
  [SD_CMD16_INDEX]  = STATIC_CONSTRUCT_COMMAND(SD_CMD16,  512,       0x00), ///< Set BLOCKLEN to 512, no CRC
  [SD_CMD9_INDEX]   = STATIC_CONSTRUCT_COMMAND(SD_CMD9,   0x00,      0x00), ///< No argument, no CRC
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
 * Repeatedly receives bytes until the desired data token is received, or until
 * an error token is.
 */
static int sd_read_data_token(sd_dev_s *dev, uint8_t token);

/**
 * Send all initialization commands to the SD card.
 */
static int sd_init_command_sequence(sd_dev_s *dev);

/**
 * Handles a full CMD0 SPI transaction.
 *
 * Returns -ETIMEDOUT if the transaction times out.
 * Returns -EAGAIN if there is an unexpected response.
 */
static int sd_cmd0_transaction(sd_dev_s *dev);

/**
 * Handles a full CMD8 SPI transaction.
 *
 * Returns -ETIMEDOUT if the transaction times out.
 * Returns -EAGAIN if there is an unexpected response.
 */
static int sd_cmd8_transaction(sd_dev_s *dev);

/**
 * Handles a full CMD55 SPI transaction.
 *
 * Returns -ETIMEDOUT if the transaction times out.
 * Returns -EAGAIN if there is an unexpected response.
 */
static int sd_cmd55_transaction(sd_dev_s *dev);

/**
 * Handles a full ACMD41 SPI transaction, including the CMD55 call.
 * Loops until the idle state bit is 0.
 *
 * Returns -ETIMEDOUT if the transaction times out.
 * Returns -EAGAIN if there is an unexpected response.
 */
static int sd_acmd41_transaction(sd_dev_s *dev);

/**
 * Handles a full CMD58 SPI transaction.
 *
 * Returns -ETIMEDOUT if the transaction times out.
 * Returns -EAGAIN if there is an unexpected response.
 */
static int sd_cmd58_transaction(sd_dev_s *dev);

/**
 * Handles a full CMD16 SPI transaction.
 * Sets block length to 512 bytes.
 *
 * Returns -ETIMEDOUT if the transaction times out.
 * Returns -EAGAIN if there is an unexpected response.
 */
static int sd_cmd16_transaction(sd_dev_s *dev);

/**
 * Handles a full CMD9 SPI transaction.
 * Unpacks necessary information from the CSD register into dev
 * Also sets any dev->prop properties that depend on CSD register values.
 *
 * Returns -ETIMEDOUT if the transaction times out.
 * Returns -EAGAIN if there is an unexpected response.
 */
static int sd_cmd9_transaction(sd_dev_s *dev);

/**
 * Simple integer power function used to calculate some of the SD card
 * properties.
 */
static uint32_t sd_pow(uint8_t base, uint16_t power);

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

    ret = sd_init_command_sequence(dev);
  }

  // Return frequency to default 2 MHz.
  spi_set_freq(dev->channel, 2000);

  return ret;
}

int sd_read_block(sd_dev_s *dev, uint32_t sector, uint8_t *buf) {
  if (sector > dev->prop.sector_count) return -EINVAL;

  int ret;
 
  uint32_t addr;

  // Set sector address
  switch (dev->capacity_class) {
    case SD_CAPACITY_STANDARD:
      addr = sector * dev->prop.sector_size;
      break;
    default:
    case SD_CAPACITY_HIGH:
      addr = sector;
      break;
  }

  sd_command_frame_s cmd = sd_construct_command(SD_CMD17, addr);
  sd_response_frame_s response;
  uint8_t discard[2];

  gpio_write(&dev->cs, GPIO_LOW);

  // Send CMD9 to read one block.
  ret = sd_write_command(dev, &cmd);

  if (ret >= 0) {
    // Receive R1 response.
    ret = sd_read_response(dev, SD_R1, &response);
  }

  if (ret >= 0) {
    // Check for response errors.
    if (response.bytes[0] != 0x00) {
      ret = -EAGAIN;
    }
  }

  if (ret >= 0) {
    // Receive data token.
    ret = sd_read_data_token(dev, TOKEN_CMD17_18_24);
  }

  if (ret >= 0) {
    // Receieve data block.
    ret = sd_readn_raw(dev, buf, dev->prop.sector_size);
  }

  if (ret >= 0) {
    // Receive and discard CRC
    ret = sd_readn_raw(dev, discard, 2);
  }

  gpio_write(&dev->cs, GPIO_HIGH);

  return ret;
}

int sd_write_block(sd_dev_s *dev, uint32_t sector, const uint8_t *buf) {
  (void)dev;
  (void)sector;
  (void)buf;

  return -EPERM;
}

int sd_readn_block(sd_dev_s *dev, uint32_t sector, uint8_t *buf, size_t n) {
  (void)dev;
  (void)sector;
  (void)buf;
  (void)n;

  return -EPERM;
}

int sd_writen_block(sd_dev_s *dev, uint32_t sector, const uint8_t *buf, size_t n) {
  (void)dev;
  (void)sector;
  (void)buf;
  (void)n;

  return -EPERM;
}

static inline sd_command_frame_s sd_construct_command(sd_cmd_e cmd, uint32_t arg) {
  return (sd_command_frame_s){
    .bytes = {
      0x0 << START_POS | 0x1 << TRAN_POS | cmd << CMD_POS,
      (arg >> 24)& 0xFF,
      (arg >> 16) & 0xFF,
      (arg >> 8) & 0xFF,
      arg & 0xFF,
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
  int ret;
  uint8_t discard;

  // Read the first byte of the response.
  do {
    ret = sd_readn_raw(dev, rx->bytes, 1);

    if (ret < 0) {
      break;
    }
  } while (rx->bytes[0] & 0x80);

  if (ret >= 0) {
    // Read the remaining bytes of the response.
    switch (type) {
      case SD_R1:
        // Do nothing.
        break;

      case SD_R1B:
        // Wait until the busy signal finishes.
        do {
          ret = sd_readn_raw(dev, &discard, 1);

          if (ret < 0) {
            break;
          }
        } while (discard == 0x00);
        break;

      case SD_R2:
        ret = sd_readn_raw(dev, &rx->bytes[1], 1);
        break;

      case SD_R3:
        ret = sd_readn_raw(dev, &rx->bytes[1], 4);
        break;

      case SD_R7:
        ret = sd_readn_raw(dev, &rx->bytes[1], 4);
        break;
    }
  }

  return ret;
}

static int sd_read_data_token(sd_dev_s *dev, uint8_t token) {
  int ret;
  uint8_t discard;

  // Wait until a valid data token is received.
  do {
    ret = sd_readn_raw(dev, &discard, 1);

    if (ret < 0) break;

    // Check for error token.
    if (~discard & 0xE0) {
      ret = -EAGAIN;
    }
  } while (discard != token);

  return ret;
}

static int sd_init_command_sequence(sd_dev_s *dev) {
  int ret;

  // Restart card in SPI mode.
  ret = sd_cmd0_transaction(dev);

  if (ret >= 0) {
    // Detect card version, and confirm voltage range is acceptable for version 2.0+ cards.
    ret = sd_cmd8_transaction(dev);
  }

  if (ret >= 0) {
    // Initialize the card.
    ret = sd_acmd41_transaction(dev);
  }

  if (ret >= 0) {
    if (dev->version == SD_VER_2X) {
      // Get card capacity class.
      ret = sd_cmd58_transaction(dev);
    }
  }

  if (ret >= 0) {
    // Set block length to 512.
    ret = sd_cmd16_transaction(dev);
    dev->prop.sector_size = 512;
  }

  if (ret >= 0) {
    // Read CSD register.
    ret = sd_cmd9_transaction(dev);
  }

  return ret;
}

static int sd_cmd0_transaction(sd_dev_s *dev) {
  int ret;
  sd_response_frame_s response;

  gpio_write(&dev->cs, GPIO_LOW);

  // Send CMD0 to reset into SPI mode.
  ret = sd_write_command(dev, &commands[SD_CMD0_INDEX]);

  if (ret >= 0) {
    // Receive R1 response.
    ret = sd_read_response(dev, SD_R1, &response);
  }

  gpio_write(&dev->cs, GPIO_HIGH);

  if (ret >= 0) {
    // Check that only the idle state bit is set.
    if (response.bytes[0] != R1_IDLE_STATE) {
      ret = -EAGAIN;
    }
  }

  return ret;
}

static int sd_cmd8_transaction(sd_dev_s *dev) {
  int ret;
  sd_response_frame_s response;

  gpio_write(&dev->cs, GPIO_LOW);

  // Send CMD8.
  ret = sd_write_command(dev, &commands[SD_CMD8_INDEX]);

  if (ret >= 0) {
    // Receive R7 response.
    ret = sd_read_response(dev, SD_R7, &response);
  }

  gpio_write(&dev->cs, GPIO_HIGH);

  if (ret >= 0) {
    // Check if the it is an illegal command.
    if (response.bytes[1] & R1_ILLEGAL_COMMAND) {
      dev->version = SD_VER_1X;
      dev->capacity_class = SD_CAPACITY_STANDARD;
    }
    else {
      dev->version = SD_VER_2X;

      // Check that only the idle state bit is set, after masking the illegal command bit.
      if ((response.bytes[0] & ~R1_ILLEGAL_COMMAND) != R1_IDLE_STATE) {
        ret = -EAGAIN;
      };
    } 
  }

  if (ret >= 0) {
    // Check that the accepted voltage is correct
    if ((response.bytes[3] & 0x0F) != 0x01) {
      ret = -EAGAIN;
    }

    // Check that the check pattern is correct
    else if (response.bytes[4] != 0x00) {
      ret = -EAGAIN;
    }
  }

  return ret;
}

static int sd_cmd55_transaction(sd_dev_s *dev) {
  int ret;
  sd_response_frame_s response;

  gpio_write(&dev->cs, GPIO_LOW);

  // Send CMD55.
  ret = sd_write_command(dev, &commands[SD_CMD55_INDEX]);

  if (ret >= 0) {
    // Receive R1 response.
    ret = sd_read_response(dev, SD_R1, &response);
  }

  gpio_write(&dev->cs, GPIO_HIGH);

  if (ret >= 0) {
    // Check that only the idle state bit is set.
    if (response.bytes[0] != R1_IDLE_STATE) {
      ret = -EAGAIN;
    }
  }

  return ret;
}

static int sd_acmd41_transaction(sd_dev_s *dev) {
  int ret;
  sd_response_frame_s response; 

  // Loop until the card is initialized.
  do {
    // Send CMD55 transaction.
    ret = sd_cmd55_transaction(dev);

    gpio_write(&dev->cs, GPIO_LOW);

    if (ret >= 0) {
      // Send ACMD41.
      ret = sd_write_command(dev, &commands[SD_ACMD41_INDEX]);
    }

    if (ret >= 0) {
      // Receive R1 response.
      ret = sd_read_response(dev, SD_R1, &response);
    }

    gpio_write(&dev->cs, GPIO_HIGH);

    if (ret >= 0) {
      // Check for errors.
      if ((response.bytes[0] & ~R1_IDLE_STATE) != 0x00) {
        ret = -EAGAIN;
      }
    }

    if (ret < 0) break;
  } while (response.bytes[0] & R1_IDLE_STATE);

  return ret;
}

static int sd_cmd58_transaction(sd_dev_s *dev) {
  int ret;
  sd_response_frame_s response;

  gpio_write(&dev->cs, GPIO_LOW);

  // Send CMD58.
  ret = sd_write_command(dev, &commands[SD_CMD58_INDEX]);

  if (ret >= 0) {
    // Receive R3 response.
    ret = sd_read_response(dev, SD_R3, &response);
  }

  gpio_write(&dev->cs, GPIO_HIGH);

  if (ret >= 0) {
    // Check for errors.
    if (response.bytes[0] != 0x00) {
      ret = -EAGAIN;
    }
  }

  if (ret >= 0) {
    // Set capacity class.
    if (response.bytes[1] & OCR_CCS) {
      dev->capacity_class = SD_CAPACITY_HIGH;
    }
    else {
      dev->capacity_class = SD_CAPACITY_STANDARD;
    }
  }

  return ret;
}

static int sd_cmd16_transaction(sd_dev_s *dev) {
  int ret;
  sd_response_frame_s response;

  gpio_write(&dev->cs, GPIO_LOW);

  ret = sd_write_command(dev, &commands[SD_CMD16_INDEX]);

  if (ret >= 0) {
    ret = sd_read_response(dev, SD_R1, &response);
  }

  gpio_write(&dev->cs, GPIO_LOW);

  if (ret >= 0) {
    // Check for errors.
    if (response.bytes[0] != 0x00) {
      ret = -EAGAIN;
    }
  }

  return ret;
}

static int sd_cmd9_transaction(sd_dev_s *dev) {
  int ret;
  sd_response_frame_s response;

  uint8_t buf[16];
  uint8_t discard[2];

  gpio_write(&dev->cs, GPIO_LOW);

  ret = sd_write_command(dev, &commands[SD_CMD9_INDEX]);

  if (ret >= 0) {
    ret = sd_read_response(dev, SD_R1, &response);
  }

  if (ret >= 0) {
    // Check for errors in the R1 response.
    if (response.bytes[0] != 0x00) {
      ret = -EAGAIN;
    }
  }

  if (ret >= 0) {
    // Receive data token.
    ret = sd_read_data_token(dev, TOKEN_CMD17_18_24);
  }

  if (ret >= 0) {
    // Read the data packet into buf.
    ret = sd_readn_raw(dev, buf, 16);
  }

  if (ret >= 0) {
    // Receive and discard CRC.
    ret = sd_readn_raw(dev, discard, 2);
  }

  gpio_write(&dev->cs, GPIO_HIGH);

  if (ret >= 0) {
    // Unpack CSD into dev.
    switch (dev->capacity_class) {
      case SD_CAPACITY_STANDARD:
        dev->csd.read_bl_len = buf[5] & 0x0F;
        dev->csd.c_size      = ((buf[6] & 0x03) << 10) | (buf[7] << 2) | (buf[8] & 0xC0 >> 6);
        dev->csd.c_size_mult = ((buf[9] & 0x03) << 1) | ((buf[10] & 0x80) >> 7);
        dev->csd.sector_size = ((buf[10] & 0x3F) << 1) | ((buf[11] & 0x80) >> 7);

        dev->prop.capacity_bytes = (dev->csd.c_size + 1) *
                                   sd_pow(2, dev->csd.c_size_mult + 2) *
                                   sd_pow(2, dev->csd.read_bl_len);
        dev->prop.sector_count = dev->prop.capacity_bytes / dev->prop.sector_size;
        dev->prop.erase_sector_size = dev->csd.sector_size + 1;
        break;

      case SD_CAPACITY_HIGH:
        // TODO implement high capacity support
        ret = -EPERM;
        break;
    }
  }

  gpio_write(&dev->cs, GPIO_LOW);

  return ret;
}

static uint32_t sd_pow(uint8_t base, uint16_t power) {
  uint32_t ret = 1;

  for (int i = 0; i < power; i++) {
    ret *= base;
  }

  return ret;
}
