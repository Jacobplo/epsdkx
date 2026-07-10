#include <epsdkx/uart.h>

#include <epsdkx/hal/uart.h>
#include <epsdkx/common/uart.h>


int uart_init(uart_channel_t channel, uint32_t baud_rate) {
  return hal_uart_init(channel, baud_rate);
}

const uart_pins_s *uart_get_pins(uart_channel_t channel) {
  return hal_uart_get_pins(channel);
}

void uart_putc(uart_channel_t channel, char chr) {
  hal_uart_putc(channel, chr);
}

void uart_write(uart_channel_t channel, const char *str) {
  hal_uart_write(channel, str);
}

char uart_getc(uart_channel_t channel) {
  return hal_uart_getc(channel);
}
