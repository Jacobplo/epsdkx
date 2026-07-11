#include <epsdkx/common/gpio.h>
#include <epsdkx/hal/spi.h>

#include <epsdkx/hal/gpio.h>
#include <epsdkx/generated/config.h>
#include "private/dma.h"
#include "private/nvic.h"

#include <errno.h>
#include <stdint.h>
#include <stddef.h>

#include "stm32f103xb.h"
#include "stm32f1xx.h"


#ifndef CONFIG_GPIO
#error Must enable CONFIG_GPIO to use SPI
#endif

#define SPI_CHANNEL_IDX(n) ((n) - 1)
#define SPI_CHANNEL_COUNT  2

#define SPI_RX_BUFFER_SIZE 64

typedef volatile struct hal_spi_rx_buffer_s {
  uint8_t data[SPI_RX_BUFFER_SIZE];
  uint16_t tail_idx;
  uint16_t head_idx;
} hal_spi_rx_buffer_s;

typedef struct hal_spi_config_s {
  spi_pins_s pins;
  SPI_TypeDef *reg;
  hal_nvic_line_device_e irq;
  hal_spi_rx_buffer_s rx_buf;
} hal_spi_config_s;

static hal_spi_config_s spi_pin_map[SPI_CHANNEL_COUNT] = {
  [SPI_CHANNEL_IDX(SPI(1))] = (hal_spi_config_s){ 
    .pins = { 
      .nss  = PORT_PIN('A', 4),
      .sclk = PORT_PIN('A', 5),
      .miso = PORT_PIN('A', 6),
      .mosi = PORT_PIN('A', 7),
    },
    .reg = SPI1,
    .irq = NVIC_SPI1
  },
  [SPI_CHANNEL_IDX(SPI(2))] = (hal_spi_config_s){ 
    .pins = { 
      .nss  = PORT_PIN('B', 12),
      .sclk = PORT_PIN('B', 13),
      .miso = PORT_PIN('B', 14),
      .mosi = PORT_PIN('B', 15),
    },
    .reg = SPI2,
    .irq = NVIC_SPI2
  },
};

static inline void hal_spi_common_isr(spi_channel_t channel);


int hal_spi_init(spi_channel_t channel, spi_mode_e mode, spi_cpol_e cpol, spi_cpha_e cpha) {
  if (SPI_CHANNEL_IDX(channel) >= SPI_CHANNEL_COUNT) return -EINVAL;

  hal_gpio_init();

  hal_spi_config_s *cfg = &spi_pin_map[SPI_CHANNEL_IDX(channel)]; 

  // Enable clock for SPI
  switch(channel) {
    case SPI(1):
      RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;    
      break;
    case SPI(2):
      RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
      break;
  }

  // Set 8-bit data width
  cfg->reg->CR1 &= ~(SPI_CR1_DFF);

  // Set MSB first format
  cfg->reg->CR1 &= ~(SPI_CR1_LSBFIRST);

  switch (mode) {
    case SPI_MASTER:
      hal_gpio_configure(&cfg->pins.nss, GPIO_OUT);
      hal_gpio_configure(&cfg->pins.sclk, GPIO_OUT_ALT_PUSH_PULL);
      hal_gpio_configure(&cfg->pins.mosi, GPIO_OUT_ALT_PUSH_PULL);
      hal_gpio_configure(&cfg->pins.miso, GPIO_IN);

      // Set baud rate to PCLK / 32
      cfg->reg->CR1 &= ~(SPI_CR1_BR);
      cfg->reg->CR1 |= (SPI_CR1_BR_2);

      // Set software slave select
      cfg->reg->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);

      // Set master mode
      cfg->reg->CR1 |= SPI_CR1_MSTR;
      break;

    case SPI_SLAVE:
      hal_gpio_configure(&cfg->pins.nss, GPIO_IN);
      hal_gpio_configure(&cfg->pins.sclk, GPIO_IN);
      hal_gpio_configure(&cfg->pins.mosi, GPIO_IN);
      hal_gpio_configure(&cfg->pins.miso, GPIO_OUT_ALT_PUSH_PULL);

      // Set slave mode
      cfg->reg->CR1 &= ~(SPI_CR1_MSTR);
      break;
  }

  // Set clock polarity
  switch (cpol) {
    case SPI_CPOL0:
      cfg->reg->CR1 &= ~(SPI_CR1_CPOL);
      break;
    case SPI_CPOL1:
      cfg->reg->CR1 |= SPI_CR1_CPOL;
      break;
  }

  // Set clock phase
  switch (cpha) {
    case SPI_CPHA0:
      cfg->reg->CR1 &= ~(SPI_CR1_CPHA);
      break;
    case SPI_CPHA1:
      cfg->reg->CR1 |= SPI_CR1_CPHA;
      break;
  } 

  // Receiver interrupt enable
  hal_nvic_init_device(cfg->irq);
  cfg->reg->CR2 |= SPI_CR2_RXNEIE;

  // Set SPI enable
  cfg->reg->CR1 |= SPI_CR1_SPE;

  return 0;
}

const spi_pins_s *hal_spi_get_pins(spi_channel_t channel) {
  if (SPI_CHANNEL_IDX(channel) >= SPI_CHANNEL_COUNT) return NULL;

  return &spi_pin_map[SPI_CHANNEL_IDX(channel)].pins;
}

void hal_spi_put(spi_channel_t channel, uint8_t tx) {
  hal_spi_config_s *cfg = &spi_pin_map[SPI_CHANNEL_IDX(channel)];

  // Wait until data register is transferred to the transmission shift register.
  while (!(cfg->reg->SR & SPI_SR_TXE)) (void)0;

  // Write to data register
  cfg->reg->DR = tx;
}

int hal_spi_get(spi_channel_t channel, uint8_t *rx) {
  hal_spi_config_s *cfg = &spi_pin_map[SPI_CHANNEL_IDX(channel)];

  // Return if buffer is empty
  if (cfg->rx_buf.tail_idx == cfg->rx_buf.head_idx) {
    return -EAGAIN;
  }

  *rx = cfg->rx_buf.data[cfg->rx_buf.tail_idx];
  cfg->rx_buf.tail_idx = (cfg->rx_buf.tail_idx + 1) % SPI_RX_BUFFER_SIZE;

  return 0;
}

static inline void hal_spi_common_isr(spi_channel_t channel) {
  hal_spi_config_s *cfg = &spi_pin_map[SPI_CHANNEL_IDX(channel)];

  // Do not overwrite buffer if it is full
  if ((cfg->rx_buf.head_idx + 1) % SPI_RX_BUFFER_SIZE == cfg->rx_buf.tail_idx) {
    cfg->reg->DR;
    return;
  }

  // Write to the buffer
  cfg->rx_buf.data[cfg->rx_buf.head_idx] = cfg->reg->DR;
  cfg->rx_buf.head_idx = (cfg->rx_buf.head_idx + 1) % SPI_RX_BUFFER_SIZE;
}


void SPI1_IRQHandler(void) {
  hal_spi_common_isr(SPI(1));
}

void SPI2_IRQHandler(void) {
  hal_spi_common_isr(SPI(2));
}
