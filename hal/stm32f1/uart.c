#include <epsdkx/hal/uart.h>

#include <epsdkx/hal/gpio.h>
#include <epsdkx/generated/config.h>
#include "private/dma.h"
#include "private/nvic.h"

#include <stdint.h>
#include <errno.h>
#include <stddef.h>

#include "stm32f103xb.h"
#include "stm32f1xx.h"


#ifndef CONFIG_GPIO
#error Must enable CONFIG_GPIO in order to use UART
#endif


#define UART_CHANNEL_COUNT 3

#define UART_RX_BUFFER_SIZE 64

typedef struct hal_uart_rx_buffer_s {
  char data[UART_RX_BUFFER_SIZE];
  uint16_t tail_idx;
  uint16_t head_idx;
} hal_uart_rx_buffer_s;

typedef struct hal_uart_config_s {
  hal_uart_pins_s pins;
  USART_TypeDef *reg;
  hal_nvic_line_device irq;
  hal_uart_rx_buffer_s rx_buf;
} hal_uart_config_s;

static hal_uart_config_s uart_pin_map[UART_CHANNEL_COUNT] = {
  [UART(1)] = (hal_uart_config_s){ 
    .pins = { 
      .tx = PORT_PIN('A', 9),
      .rx = PORT_PIN('A', 10) 
    },
    .reg = USART1,
    .irq = NVIC_USART1
  },
  [UART(2)] = (hal_uart_config_s){ 
    .pins = { 
      .tx = PORT_PIN('A', 2),
      .rx = PORT_PIN('A', 3)
    },
    .reg = USART2,
    .irq = NVIC_USART2
  },
  [UART(3)] = (hal_uart_config_s){ 
    .pins = {
      .tx = PORT_PIN('B', 10),
      .rx = PORT_PIN('B', 11)
    },
    .reg = USART3,
    .irq = NVIC_USART3
  },
};


static void hal_uart_set_baud_rate(hal_uart_channel_t channel, uint32_t baud_rate);
static void hal_uart_common_isr(hal_uart_channel_t channel);


int hal_uart_init(hal_uart_channel_t channel, uint32_t baud_rate) {
  if (channel >= UART_CHANNEL_COUNT) return -EINVAL;

  hal_gpio_init();
  hal_dma_init();

  hal_uart_config_s *cfg = &uart_pin_map[channel]; 

  hal_gpio_configure(&cfg->pins.tx, HAL_GPIO_OUT_ALT_PUSH_PULL);
  hal_gpio_configure(&cfg->pins.rx, HAL_GPIO_IN);

  // Enable clock for UART
  switch(channel) {
    case UART(1):
      RCC->APB2ENR |= RCC_APB2ENR_USART1EN;    
      break;
    case UART(2):
      RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
      break;
    case UART(3):
      RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
      break;
  }

  // UART enable
  cfg->reg->CR1 |= USART_CR1_UE;

  // Select 8-bit word length
  cfg->reg->CR1 &= ~(USART_CR1_M);

  // Select 1 stop bit
  cfg->reg->CR2 &= ~(USART_CR2_STOP);

  // DMA enable transmitter and receiver
  // cfg.reg->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
  
  hal_uart_set_baud_rate(channel, baud_rate);

  // Transmitter enable
  cfg->reg->CR1 |= USART_CR1_TE;

  // Receiver enable
  cfg->reg->CR1 |= USART_CR1_RE;

  // Receiver interrupt enable
  hal_nvic_init_device(cfg->irq);
  cfg->reg->CR1 |= USART_CR1_RXNEIE;
  
  return 0;
}

const hal_uart_pins_s *hal_uart_get_pins(hal_uart_channel_t channel) {
  if (channel >= UART_CHANNEL_COUNT) return NULL;

  return &uart_pin_map[channel].pins;
}

void hal_uart_putc(hal_uart_channel_t channel, char chr) {
  hal_uart_config_s *cfg = &uart_pin_map[channel];

  // Wait until data register is transferred to the transmission shift register.
  while (!(cfg->reg->SR & USART_SR_TXE)) (void)0;

  // Write to data register
  cfg->reg->DR = chr;
}

void hal_uart_write(hal_uart_channel_t channel, const char *str) {
  hal_uart_config_s *cfg = &uart_pin_map[channel]; 

  while (*str != '\0') { 
    hal_uart_putc(channel, *str);
    str++;
  }

  // Wait until transmission frame is complete
  while(!(cfg->reg->SR & USART_SR_TC)) (void)0;
}

char hal_uart_getc(hal_uart_channel_t channel) {
  hal_uart_config_s *cfg = &uart_pin_map[channel];

  // Return EOF if buffer is empty
  if (cfg->rx_buf.tail_idx == cfg->rx_buf.head_idx) {
    return -1;
  }

  char chr = cfg->rx_buf.data[cfg->rx_buf.tail_idx];
  cfg->rx_buf.tail_idx = (cfg->rx_buf.tail_idx + 1) % UART_RX_BUFFER_SIZE;

  return chr;
}

void hal_uart_set_baud_rate(hal_uart_channel_t channel, uint32_t baud_rate) {
  hal_uart_config_s *cfg = &uart_pin_map[channel];

  uint32_t prescaler;
  uint8_t clock_divider = 1;

  // Get PCLK
  if (channel == UART(1)) {
    prescaler = RCC->CFGR & RCC_CFGR_PPRE2;

    switch(prescaler) {
      case(RCC_CFGR_PPRE2_DIV1):
        clock_divider = 1;
        break;
      case(RCC_CFGR_PPRE2_DIV2):
        clock_divider = 2;
        break;
      case(RCC_CFGR_PPRE2_DIV4):
        clock_divider = 4;
        break;
      case(RCC_CFGR_PPRE2_DIV8):
        clock_divider = 8;
        break;
      case(RCC_CFGR_PPRE2_DIV16):
        clock_divider = 16;
        break;
    };
  }
  else {
    prescaler = RCC->CFGR & RCC_CFGR_PPRE1;

    switch(prescaler) {
      case(RCC_CFGR_PPRE1_DIV1):
        clock_divider = 1;
        break;
      case(RCC_CFGR_PPRE1_DIV2):
        clock_divider = 2;
        break;
      case(RCC_CFGR_PPRE1_DIV4):
        clock_divider = 4;
        break;
      case(RCC_CFGR_PPRE1_DIV8):
        clock_divider = 8;
        break;
      case(RCC_CFGR_PPRE1_DIV16):
        clock_divider = 16;
        break;
    };
  }

  uint32_t pclk = SystemCoreClock / clock_divider;

  float usart_div = (float)pclk / (16u * baud_rate);
  uint32_t usart_div_int = (uint32_t)usart_div;
  float usart_div_frac = usart_div - usart_div_int; 

  // Get fixed point representation of the fraction, rounded to the nearest integer.
  uint32_t div_fraction = (uint32_t)(16u * usart_div_frac + 0.5f);
  uint32_t div_mantissa = usart_div_int;

  // Add the carry from the fraction to the mantissa, if applicable
  if (div_fraction >= 16u) {
    div_mantissa += 1u;
  } 

  cfg->reg->BRR = ((div_mantissa << USART_BRR_DIV_Mantissa_Pos) & USART_BRR_DIV_Mantissa_Msk) |
                  ((div_fraction << USART_BRR_DIV_Fraction_Pos) & USART_BRR_DIV_Fraction_Msk);
}

void hal_uart_common_isr(hal_uart_channel_t channel) {
  hal_uart_config_s *cfg = &uart_pin_map[channel];

  // Do not overwrite buffer if it is full
  if ((cfg->rx_buf.head_idx + 1) % UART_RX_BUFFER_SIZE == cfg->rx_buf.tail_idx) {
    cfg->reg->DR;
    return;
  }

  // Write the buffer
  cfg->rx_buf.data[cfg->rx_buf.head_idx] = cfg->reg->DR;
  cfg->rx_buf.head_idx = (cfg->rx_buf.head_idx + 1) % UART_RX_BUFFER_SIZE;
}

void USART1_IRQHandler() {
  hal_uart_common_isr(UART(1));
}

void USART2_IRQHandler() {
  hal_uart_common_isr(UART(2));
}

void USART3_IRQHandler() {
  hal_uart_common_isr(UART(3));
}
