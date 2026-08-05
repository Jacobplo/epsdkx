/**
 * Public API for the I2C interface driver.
 *
 * Master / slave wrappers for the mode-independent I2C functions are defined
 * at the bottom of the file, and should be used instead of the generic
 * functions to make intent explicit.
 *
 * Unless otherwise stated, all functions that return int return a negative
 * errno on error, and 0 on success.
 */

#ifndef _EPSDKX_I2C_H
#define _EPSDKX_I2C_H

#include <epsdkx/common/i2c.h>
#include <epsdkx/hal/i2c.h>

#include <stdint.h>

/**
 * Generic I2C interface intialization that works for either master or slave
 * mode.
 *
 * Prefer using the i2c_init_master() / i2c_init_slave() wrappers to make 
 * intent more explicit.
 */
int i2c_init(i2c_channel_t channel, i2c_mode_e mode, uint16_t address);

/**
 * Transmits a contiguous chunk of n bytes of data in a single transmission.
 * Works in both master and slave mode.
 *
 * Prefer using the i2c_writen_master() / i2c_writen_slave() wrappers to make 
 * intent more explicit.
 */
int i2c_writen(i2c_channel_t channel, uint8_t *tx, uint16_t n, uint16_t slave_address);

/**
 * Receives a contiguous chunk of n bytes of data in a single transmission
 * into an internal RX buffer. Works in both master and slave mode.
 *
 * Prefer using the i2c_readn_master() / i2c_readn_slave() wrappers to make 
 * intent more explicit.
 */
int i2c_readn(i2c_channel_t channel, uint16_t n, uint16_t slave_address);

/**
 * Transmits a single byte of data. Works in both master and slave mode.
 *
 * Prefer using the i2c_write_master() / i2c_write_slave() wrappers to make 
 * intent more explicit.
 */
int i2c_write(i2c_channel_t channel, uint8_t *tx, uint16_t slave_address);

/**
 * Receives a single byte of data into an internal RX buffer. 
 * Works in both master and slave mode.
 *
 * Prefer using the i2c_read_master() / i2c_read_slave() wrappers to make 
 * intent more explicit.
 */
int i2c_read(i2c_channel_t channel, uint16_t slave_address);

/**
 * Retrieves a single byte of data from the internal RX buffer into rx.
 */
int i2c_get(i2c_channel_t channel, uint8_t *rx);

/**
 * Returns the GPIO pins used by the I2C interface.
 */
const i2c_pins_s *i2c_get_pins(i2c_channel_t channel);

/**
 * Returns true of the bus connected to the I2C interface is in use. Returns
 * false, otherwise.
 */
bool i2c_is_busy(i2c_channel_t channel);


/*
 * Master / slave wrapper functions
 */

static inline int i2c_init_master(i2c_channel_t channel) {
  return i2c_init(channel, I2C_MASTER, 0);
}

static inline int i2c_init_slave(i2c_channel_t channel, uint16_t address) {
  return i2c_init(channel, I2C_SLAVE, address);
}

static inline int i2c_writen_master(i2c_channel_t channel, uint8_t *tx, uint16_t n, uint16_t slave_address) {
  return i2c_writen(channel, tx, n, slave_address);
}

static inline int i2c_writen_slave(i2c_channel_t channel, uint8_t *tx, uint16_t n) {
  return i2c_writen(channel, tx, n, 0);
}

static inline int i2c_readn_master(i2c_channel_t channel, uint16_t n, uint16_t slave_address) {
  return i2c_readn(channel, n, slave_address);
}

static inline int i2c_readn_slave(i2c_channel_t channel, uint16_t n) {
  return i2c_readn(channel, n, 0);
}

static inline int i2c_write_master(i2c_channel_t channel, uint8_t *tx, uint16_t slave_address) {
  return i2c_write(channel, tx, slave_address);
}

static inline int i2c_write_slave(i2c_channel_t channel, uint8_t *tx) {
  return i2c_write(channel, tx, 0);
}

static inline int i2c_read_master(i2c_channel_t channel, uint16_t slave_address) {
  return i2c_read(channel, slave_address);
}

static inline int i2c_read_slave(i2c_channel_t channel) {
  return i2c_read(channel, 0);
}

#endif
