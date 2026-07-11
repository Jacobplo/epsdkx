#include <epsdkx/drivers/uart.h>

#include <epsdkx/hal/uart.h>
#include <epsdkx/common/uart.h>


int uart_init(uart_channel_t channel, uint32_t baud_rate) {
  return hal_uart_init(channel, baud_rate);
}

const uart_pins_s *uart_get_pins(uart_channel_t channel) {
  return hal_uart_get_pins(channel);
}

void uart_put(uart_channel_t channel, uint8_t tx) {
  hal_uart_put(channel, tx);
}

int uart_get(uart_channel_t channel, uint8_t *rx) {
  return hal_uart_get(channel, rx);
}

void uart_write(uart_channel_t channel, const char *str) {
  while (*str != '\0') { 
    hal_uart_put(channel, *str);
    str++;
  }
}
