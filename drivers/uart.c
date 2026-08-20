#include <epsdkx/drivers/uart.h>

#include <epsdkx/hal/uart.h>
#include <epsdkx/common/uart.h>
#include <epsdkx/drivers/time.h>

#include <stddef.h>
#include <errno.h>


int uart_init(uart_channel_t channel, uint32_t baud_rate) {
  return hal_uart_init(channel, baud_rate);
}

const uart_pins_s *uart_get_pins(uart_channel_t channel) {
  return hal_uart_get_pins(channel);
}

int uart_write(uart_channel_t channel, uint8_t tx) {
  return hal_uart_write(channel, tx);
}

int uart_get(uart_channel_t channel, uint8_t *rx) {
  return hal_uart_get(channel, rx);
}

int uart_getn(uart_channel_t channel, uint8_t *rx, size_t n, uint32_t timeout_ms) {
  int ret = 0;

  uint32_t start = time_get_ticks();  

  for (size_t i = 0; i < n; i++) {
    while (uart_get(channel, &rx[i]) < 0) {
      uint32_t cur = time_get_ticks();
      if (time_ticks_to_ms(cur - start) > timeout_ms) {
        ret = -ETIMEDOUT;
        goto getn_return;
      }
    }
  }

getn_return:
  return ret;
}

int uart_writen(uart_channel_t channel, const uint8_t *tx, size_t n) {
  int ret;

  for (size_t i = 0; i < n; i++) { 
    if ((ret = hal_uart_write(channel, tx[i])) < 0) {
      return ret;
    };
  }

  return 0;
}

void uart_flush(uart_channel_t channel) {
  uint8_t discard;
  while (uart_get(channel, &discard) >= 0);
}
