#include <epsdkx/periph/at6558.h>
#include "private/at6558_def.h"

#include <epsdkx/drivers/uart.h>
#include <epsdkx/common/uart.h>
#include <epsdkx/drivers/time.h>
#include <epsdkx/generated/config.h>

#include <string.h>
#include <stddef.h>


#if !defined (CONFIG_UART)
#error CONFIG_UART must be enabled to use the AT6558 driver
#endif

#if CONFIG_RX_BUFFER_SIZE < AT6558_FRAME_SIZE
#error CONFIG_RX_BUFFER_SIZE must be at least 128 to use the AT6558 driver
#endif

typedef enum at6558_csip_message_e {
  AT6558_POLL_NAV_PV,

  AT6558_MESSAGE_COUNT,
} at6558_csip_message_e;

/**
 * Contains pre-defined CSIP messages used by the driver, without a checksum.
 */
static const uint8_t *const csip_messages[AT6558_MESSAGE_COUNT] = {
    [AT6558_POLL_NAV_PV] = (const uint8_t[]){CSIP_HEADER, CSIP_LEN(4), CFG_MSG,
                                             NAV_PV, 0xFF, 0xFF},
};

#define NMEA_INIT_MESSAGES_SIZE 1
static const char *const nmea_init_messages[NMEA_INIT_MESSAGES_SIZE] = {
  "$PCAS03,0,0,0,0,0,0,0,0,0,0,,,0,0,,,,0*32\r\n", // Disable automatic message output
};

/**
 * Reads a communication frame from the UART interface into dev->frame.
 */
static void at6558_read_frame(at6558_dev_s *dev);


/**
 * Writes a communication frame from dev->frame to the UART interface.
 */
static void at6558_write_frame(at6558_dev_s *dev, size_t n);

/**
 * Copies frame bytes from the provided message frame to dev->frame,
 * and also computes the frame checksum.
 *
 * Writes the frame after copying the full frame to dev->frame.
 */
static void at6558_csip_construct_frame(at6558_dev_s *dev, at6558_csip_message_e message);

static void at6558_parse_nav_pv_payload(at6558_dev_s *dev, at6558_csip_message_e message);


int at6558_init(at6558_dev_s *dev, uart_channel_t channel) {
  int ret;

  dev->channel = channel;

  // Let the device boot before communication
  time_delay_ms(1000);

  ret = uart_init(channel, 9600);
  time_delay_ms(5);
  
  if (ret >= 0) {
    // Send initialization messages to the device
    for (size_t i = 0; i < NMEA_INIT_MESSAGES_SIZE; i++) {
      size_t len = strlen(nmea_init_messages[i]);
      memcpy(dev->frame, nmea_init_messages[i], len);

      at6558_write_frame(dev, len);

      time_delay_ms(5);
    }

    uart_flush(dev->channel);
  }

  return ret;
}

static void at6558_read_frame(at6558_dev_s *dev) {
  (void)dev;  
}

static void at6558_write_frame(at6558_dev_s *dev, size_t n) {
  for (size_t i = 0; i < n; i++) {
    uart_write(dev->channel, dev->frame[i]);
  }
}

static void at6558_csip_construct_frame(at6558_dev_s *dev, at6558_csip_message_e message) {
  const uint8_t *const frame = csip_messages[message];

  const size_t payload_len = JOIN2(&frame[CSIP_LEN_POS]);

  // Message length minus the checksum. Header + length + type = 6.
  const size_t len = payload_len + 6;

  // Compute checksum
  uint32_t ckSum = (frame[CSIP_ID_POS] << 24) + (frame[CSIP_CLASS_POS] << 16) + payload_len;
  for (size_t i = 0; i < payload_len; i += 4) {
    ckSum += JOIN4(&frame[CSIP_PAYLOAD_POS + i]);
  }

  const uint8_t ckSumSplit[4] = { SPLIT4(ckSum) };

  memcpy(dev->frame, frame, len);
  memcpy(dev->frame + len, ckSumSplit, 4);

  at6558_write_frame(dev, len + 4);
}
