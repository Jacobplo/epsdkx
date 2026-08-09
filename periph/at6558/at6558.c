#include <epsdkx/drivers/time.h>
#include <epsdkx/periph/at6558.h>

#include <epsdkx/drivers/uart.h>
#include <epsdkx/common/uart.h>
#include <epsdkx/generated/config.h>

#include <string.h>
#include <stddef.h>


#if !defined (CONFIG_UART)
#error CONFIG_UART must be enabled to use the AT6558 driver
#endif

#if CONFIG_RX_BUFFER_SIZE < AT6558_MAX_FRAME_SIZE
#error CONFIG_RX_BUFFER_SIZE must be at least 82 to use the AT6558 driver
#endif


#define INIT_MESSAGES_SIZE 1
static const char *const init_messages[INIT_MESSAGES_SIZE] = {
  "$PCAS03,0,0,0,0,0,0,0,0,0,0,,,0,0,,,,0*32\r\n", // Disable automatic message output
};

static const uint8_t test[14] = { 0xBA, 0xCE, 0x04, 0x00, 0x06, 0x01, 0x01, 0x03, 0xFF, 0xFF, 0x05, 0x03, 0x05, 0x01 };

static void at6558_read_frame(at6558_dev_s *dev);
static void at6558_write_frame(at6558_dev_s *dev);


int at6558_init(at6558_dev_s *dev, uart_channel_t channel) {
  int ret;

  dev->channel = channel;

  // Let the device boot before communication
  time_delay_ms(1000);

  ret = uart_init(channel, 9600);
  time_delay_ms(5);
  
  if (ret >= 0) {
    for (size_t i = 0; i < INIT_MESSAGES_SIZE; i++) {
      strcpy(dev->frame, init_messages[i]);
      at6558_write_frame(dev);
      time_delay_ms(100);
    }

    memcpy(dev->frame, test, 14);
    at6558_write_frame(dev);
  }

  return ret;
}

static void at6558_write_frame(at6558_dev_s *dev) {
  char *ptr = dev->frame;

  while (*ptr != '\n') {
    uart_write(dev->channel, *ptr);
    ptr++;
  }
  uart_write(dev->channel, *ptr);
}
