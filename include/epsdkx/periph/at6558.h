/**
 * Public API driver for the AT6558 GNSS receiver.
 *
 * Unless otherwise stated, all functions that return int return a
 * negative errno on error, and 0 on success.
 */

#ifndef _EPSDKX_PERIPH_AT6558_H
#define _EPSDKX_PERIPH_AT6558_H

#include <epsdkx/common/uart.h>


#define AT6558_FRAME_SIZE 128

/**
 * Used to store information about a specific AT6558 device in user-space
 */
typedef struct at6558_dev_s {
  uart_channel_t channel;
  uint8_t frame[AT6558_FRAME_SIZE];
} at6558_dev_s;

/**
 * Error variances for geodetic coordinate system fields
 */
typedef struct at6558_variance_s {
  float horizontal_pos; ///< Variance of horizontal position error in m^2
  float vertical_pos;   ///< Variance of vertical position error in m^2
  float ground_speed;   ///< Variance of ground velocity speed error in (m/s)^2
  float heading;        ///< Variance of heading error in (deg)^2
} at6558_variance_s;

/**
 * UTC time stamp of a particular GNSS datum
 */
typedef struct at6558_utc_time_s {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint16_t ms;
} at6558_utc_time_s;

/**
 * Geodetic coordinate system position and velocity information
 */
typedef struct at6558_datum_s {
  float dop;          ///< Dilution of position
  double lon;         ///< Geodetic longitude in degrees
  double lat;         ///< Geodetic latitude in degrees
  float height;       ///< Geodetic height in meters
  float vel_north;    ///< North velocity in meters per second
  float vel_east;     ///< East velocity in meters per second
  float vel_up;       ///< Vertical velocity in meters per second
  float speed;        ///< Speed in meters per second
  float ground_speed; ///< Ground speed in meters per second
  float heading;      ///< Heading in degrees
  at6558_variance_s var;
  at6558_utc_time_s time;
} at6558_datum_s;

/**
 * Initializes the provided UART channel for use with the GNSS receiver and
 * sends configuration messages to the device for use with this driver.
 *
 * dev should be declared empty by the user. It stores data about the device
 * and interface.
 */
int at6558_init(at6558_dev_s *dev, uart_channel_t channel);

/**
 * Polls the GNSS receiver for geodetic coordinate system information.
 */
int at6558_get_gnss_datum(at6558_dev_s *dev, at6558_datum_s *datum);

/**
 * Instructs the device to enter the standby low power mode for the provided
 * number of seconds.
 */
void at6558_standby(at6558_dev_s *dev, uint16_t seconds);

#endif
