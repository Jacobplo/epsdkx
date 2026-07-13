#ifndef _EPSDKX_HAL_RX_BUFFER_H
#define _EPSDKX_HAL_RX_BUFFER_H

#include <stdint.h>
#include <errno.h>
#include <stdbool.h>

#define RX_BUFFER_SIZE 64

typedef volatile struct hal_rx_buffer_s {
  uint8_t data[RX_BUFFER_SIZE];
  uint16_t tail_idx;
  uint16_t head_idx;
} hal_rx_buffer_s;

static inline bool hal_rx_buffer_is_full(hal_rx_buffer_s *rx_buf) {
   return (rx_buf->head_idx + 1) % RX_BUFFER_SIZE == rx_buf->tail_idx;
}

static inline bool hal_rx_buffer_is_empty(hal_rx_buffer_s *rx_buf) {
  return rx_buf->tail_idx == rx_buf->head_idx;
}

static inline int hal_rx_buffer_get(hal_rx_buffer_s *rx_buf, uint8_t *rx) {
  // Return if buffer is empty
  if (hal_rx_buffer_is_empty(rx_buf)) {
    return -EAGAIN;
  }

  *rx = rx_buf->data[rx_buf->tail_idx];
  rx_buf->tail_idx = (rx_buf->tail_idx + 1) % RX_BUFFER_SIZE;

  return 0;
}

static inline void hal_rx_buffer_put(hal_rx_buffer_s *rx_buf, uint8_t data) {
  // Do not overwrite buffer if it is full
  if (hal_rx_buffer_is_full(rx_buf)) {
    return;
  }

  // Write to the buffer
  rx_buf->data[rx_buf->head_idx] = data;
  rx_buf->head_idx = (rx_buf->head_idx + 1) % RX_BUFFER_SIZE;
}

#endif
