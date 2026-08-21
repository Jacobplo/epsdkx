/**
 * Primarily interfaces with the AT6558 device using the CASIC Multimode
 * Satellite Navigation Receiver Protocol / CASIC Standard Interface Protocol
 * (CSIP). CSIP is used in this driver to poll data from the device, which will
 * also respond with ACK for success, or NACK for failure.
 *
 * The NMEA 0183 protocol can also be used to a more limited degree, but this
 * driver only uses proprietary messages that are not provided by the CASIC
 * protocol.
 */

#include <epsdkx/periph/at6558.h>
#include "private/at6558_def.h"

#include <epsdkx/drivers/uart.h>
#include <epsdkx/common/uart.h>
#include <epsdkx/drivers/time.h>
#include <epsdkx/generated/config.h>

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>


#if !defined (CONFIG_UART)
#error CONFIG_UART must be enabled to use the AT6558 driver
#endif

#if CONFIG_RX_BUFFER_SIZE < AT6558_FRAME_SIZE
#error CONFIG_RX_BUFFER_SIZE must be at least 128 to use the AT6558 driver
#endif

typedef enum at6558_csip_message_e {
  AT6558_POLL_NAV_PV,
  AT6558_POLL_NAV_TIMEUTC,
  AT6558_MAX_RATE,

  AT6558_MESSAGE_COUNT,
} at6558_csip_message_e;

/**
 * Contains pre-defined CSIP messages used by the driver, without a checksum.
 */
static const uint8_t *const csip_messages[AT6558_MESSAGE_COUNT] = {
  [AT6558_POLL_NAV_PV]      = (const uint8_t[]){CSIP_HEADER, CSIP_LEN(4), CFG_MSG,
                                                NAV_PV, 0xFF, 0xFF},
  [AT6558_POLL_NAV_TIMEUTC] = (const uint8_t[]){CSIP_HEADER, CSIP_LEN(4), CFG_MSG,
                                                NAV_TIMEUTC, 0xFF, 0xFF},
  [AT6558_MAX_RATE]         = (const uint8_t[]){CSIP_HEADER, CSIP_LEN(4), CFG_RATE,
                                                0x64, 0x00, 0x00, 0x00}
};

#define NMEA_INIT_MESSAGES_SIZE 1
static const char *const nmea_init_messages[NMEA_INIT_MESSAGES_SIZE] = {
  "$PCAS03,0,0,0,0,0,0,0,0,0,0,,,0,0,,,,0*32\r\n", // Disable automatic message output
};

/**
 * Reads a communication frame from the UART interface into dev->frame.
 */
static void at6558_read_frame(at6558_dev_s *dev, size_t n);


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

/**
 * Computes and returns the checksum for a CSIP protocol frame
 */
static uint32_t at6558_csip_compute_checksum(const uint8_t *frame);

/**
 * Parses dev->frame as if its an ACK message type.
 *
 * Returns -EAGAIN if dev->frame is not an ACK message.
 * Returns -EAGAIN if dev->frame is an ACK-NACK message. 
 * Retruns -EAGAIN if the checksum does not match.
 * Returns 0 otherwise.
 */
static int at6558_parse_ack(at6558_dev_s *dev);


/**
 * Parses dev->frame as if its a NAV-PV message type, cleanly into datum fields.
 *
 * Returns -EAGAIN if dev->frame is not a NAV-PV message.
 * Returns -EAGAIN if the checksum does not match.
 * Returns -EAGAIN if the payload has invalid data.
 * Returns 0 otherwise.
 */
static int at6558_parse_nav_pv(at6558_dev_s *dev, at6558_datum_s *datum);

/**
 * Parses dev->frame as if its a NAV-TIMEUTC message type, cleanly into datum
 * fields.
 *
 * Returns -EAGAIN if dev->frame is not a NAV-TIMEUTC message.
 * Returns -EAGAIN if the checksum does not match.
 * Returns -EAGAIN if the payload has invalid data.
 * Returns 0 otherwise.
 */
static int at6558_parse_nav_timeutc(at6558_dev_s *dev, at6558_utc_time_s *time);

/**
 * Interprets and return a 32-bit integer bitfield as a float.
 */
static float at6558_extract_float(const uint32_t bits);

/**
 * Interprets and return a 64-bit integer bitfield as a double.
 */
static double at6558_extract_double(const uint64_t bits);

/**
 * Computes and returns the checksum for a NMEA protocol frame
 */
static uint32_t at6558_nmea_compute_checksum(const uint8_t *frame);


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

    // Set 10 Hz navigation rate
    at6558_csip_construct_frame(dev, AT6558_MAX_RATE);
    at6558_read_frame(dev, AT6558_LEN_ACK);
    ret = at6558_parse_ack(dev);
  }

  return ret;
}

int at6558_get_gnss_datum(at6558_dev_s *dev, at6558_datum_s *datum) {
  int ret = 0;

  // Poll for UTC time
  at6558_csip_construct_frame(dev, AT6558_POLL_NAV_TIMEUTC);

  // Receive and parse ACK message
  at6558_read_frame(dev, AT6558_LEN_ACK); 
  ret = at6558_parse_ack(dev);

  if (ret >= 0) {
    // Receive and parse NAV-TIMEUTC message
    at6558_read_frame(dev, AT6558_LEN_NAV_TIMEUTC);
    ret = at6558_parse_nav_timeutc(dev, &datum->time);
  }

  if (ret >= 0) {
    // Poll for geodetic position and velocity information
    at6558_csip_construct_frame(dev, AT6558_POLL_NAV_PV);

    // Receive and parse ACK message
    at6558_read_frame(dev, AT6558_LEN_ACK); 
    ret = at6558_parse_ack(dev);
  }

  if (ret >= 0) {
  // Receive and parse NAV-PV message
    at6558_read_frame(dev, AT6558_LEN_NAV_PV);
    ret = at6558_parse_nav_pv(dev, datum);
  }

  return ret;
}

void at6558_standby(at6558_dev_s *dev, uint16_t seconds) {
  size_t i = 0;

  i += sprintf((char *)&dev->frame[i], "$PCAS12");
  i += sprintf((char *)&dev->frame[i], ",%d", seconds);
  i += sprintf((char *)&dev->frame[i], "*");

  const uint8_t cksum = at6558_nmea_compute_checksum(dev->frame);

  i += sprintf((char *)&dev->frame[i], "%02X\r\n", cksum);

  at6558_write_frame(dev, i);
}

static void at6558_read_frame(at6558_dev_s *dev, size_t n) {
  for (size_t i = 0; i < n; i++) {
    while (uart_get(dev->channel, dev->frame + i) < 0) (void)0;
  }
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

  const uint32_t cksum = at6558_csip_compute_checksum(frame);
  const uint8_t cksum_split[4] = { SPLIT4(cksum) };

  memcpy(dev->frame, frame, len);
  memcpy(dev->frame + len, cksum_split, 4);

  at6558_write_frame(dev, len + 4);
}

static uint32_t at6558_csip_compute_checksum(const uint8_t *frame) {
  const size_t payload_len = JOIN2(&frame[CSIP_LEN_POS]);

  uint32_t cksum = (frame[CSIP_ID_POS] << 24) + (frame[CSIP_CLASS_POS] << 16) + payload_len;
  for (size_t i = 0; i < payload_len; i += 4) {
    cksum += JOIN4(&frame[CSIP_PAYLOAD_POS + i]);
  }

  return cksum;
}

static int at6558_parse_ack(at6558_dev_s *dev) {
  int ret = 0;

  const uint32_t computed_cksum = at6558_csip_compute_checksum(dev->frame);
  const uint32_t received_cksum = JOIN4(&dev->frame[AT6558_LEN_ACK - 4]);

  const at6558_cisp_class_e class = dev->frame[CSIP_CLASS_POS];
  const at6558_cisp_id_e id = dev->frame[CSIP_ID_POS];

  if (computed_cksum != received_cksum) {
    ret = -EAGAIN;
  }
  else if (class != AT6558_ACK) {
    ret = -EAGAIN;
  }
  else if (id == AT6558_ACK_NACK) {
    ret = -EAGAIN;
  }

  return ret;
}

static int at6558_parse_nav_pv(at6558_dev_s *dev, at6558_datum_s *datum) {
  int ret = 0;

  const uint32_t computed_cksum = at6558_csip_compute_checksum(dev->frame);
  const uint32_t received_cksum = JOIN4(&dev->frame[AT6558_LEN_NAV_PV - 4]);

  const at6558_cisp_class_e class = dev->frame[CSIP_CLASS_POS];
  const at6558_cisp_id_e id = dev->frame[CSIP_ID_POS];

  const uint8_t *payload = &dev->frame[CSIP_PAYLOAD_POS];

  if (computed_cksum != received_cksum) {
    ret = -EAGAIN;
  }
  else if (class != AT6558_NAV && id != AT6558_NAV_PV) {
    ret = -EAGAIN;
  }
 
  // Checks if positio and velocity are valid
  if (ret >= 0) {
    if (payload[4] == 0x00) ret = -EAGAIN;
    if (payload[5] == 0x00) ret = -EAGAIN;
  }

  if (ret >= 0) {
    datum->dop                = at6558_extract_float(JOIN4(&payload[12]));
    datum->lon                = at6558_extract_double(JOIN8(&payload[16]));
    datum->lat                = at6558_extract_double(JOIN8(&payload[24]));
    datum->height             = at6558_extract_float(JOIN4(&payload[32]));
    datum->var.horizontal_pos = at6558_extract_float(JOIN4(&payload[40]));
    datum->var.vertical_pos   = at6558_extract_float(JOIN4(&payload[44]));
    datum->vel_north          = at6558_extract_float(JOIN4(&payload[48]));
    datum->vel_east           = at6558_extract_float(JOIN4(&payload[52]));
    datum->vel_up             = at6558_extract_float(JOIN4(&payload[56]));
    datum->speed              = at6558_extract_float(JOIN4(&payload[60]));
    datum->ground_speed       = at6558_extract_float(JOIN4(&payload[64]));
    datum->heading            = at6558_extract_float(JOIN4(&payload[68]));
    datum->var.ground_speed   = at6558_extract_float(JOIN4(&payload[72]));
    datum->var.heading        = at6558_extract_float(JOIN4(&payload[76]));
  }

  return ret;
}

static int at6558_parse_nav_timeutc(at6558_dev_s *dev, at6558_utc_time_s *time) {
  int ret = 0;

  const uint32_t computed_cksum = at6558_csip_compute_checksum(dev->frame);
  const uint32_t received_cksum = JOIN4(&dev->frame[AT6558_LEN_NAV_TIMEUTC - 4]);

  const at6558_cisp_class_e class = dev->frame[CSIP_CLASS_POS];
  const at6558_cisp_id_e id = dev->frame[CSIP_ID_POS];

  const uint8_t *payload = &dev->frame[CSIP_PAYLOAD_POS];

  if (computed_cksum != received_cksum) {
    ret = -EAGAIN;
  }
  else if (class != AT6558_NAV && id != AT6558_NAV_TIMEUTC) {
    ret = -EAGAIN;
  }
 
  // Checks if time and date are valid
  if (ret >= 0) {
    if (!(payload[21] & 0x07)) ret = -EAGAIN;
    if (payload[23] == 0x00) ret = -EAGAIN;
  }

  if (ret >= 0) {
    time->ms    = JOIN2(&payload[12]);
    time->year  = JOIN2(&payload[14]);
    time->month = payload[16];
    time->day   = payload[17];
    time->hour  = payload[18];
    time->min   = payload[19];
    time->sec   = payload[20];
  }

  return ret;
}

static float at6558_extract_float(const uint32_t bits) {
  float ret;
  memcpy(&ret, &bits, sizeof(bits));
  return ret;
}

static double at6558_extract_double(const uint64_t bits) {
  double ret;
  memcpy(&ret, &bits, sizeof(bits));
  return ret;
}

static uint32_t at6558_nmea_compute_checksum(const uint8_t *frame) {
  uint8_t cksum = 0x00;

  // Set first character to the character following '$'
  const uint8_t *cur = &frame[1];

  uint8_t chr;
  while ((chr = *cur) != '*') {
    cksum ^= chr;
    cur++;
  }

  return cksum;
}
