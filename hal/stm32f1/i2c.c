#include <epsdkx/common/i2c.h>
#include <epsdkx/hal/i2c.h>

#include <epsdkx/hal/gpio.h>
#include <epsdkx/generated/config.h>
#include "private/nvic.h"
#include "private/rx_buffer.h"

#include <epsdkx/hal/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <stddef.h>

#include "stm32f103xb.h"
#include "stm32f1xx.h"


#ifndef CONFIG_GPIO
#error Must enable CONFIG_GPIO to use GPIO
#endif

#ifndef CONFIG_STM32F1_I2C_CCR
#error Must set CONFIG_STM32F1_I2C_CCR in the board Kconfig, as defined in the datasheet
#endif

#define I2C_CHANNEL_IDX(n) ((n) - 1)
#define I2C_CHANNEL_COUNT  2

typedef volatile struct hal_i2c_data_s{
  // Master
  uint16_t target;
  uint8_t *tx;

  // Slave
  hal_rx_buffer_s rx_buf;

  uint16_t i;
  uint16_t n;
} hal_i2c_data_s;

typedef struct hal_i2c_config_s {
  i2c_pins_s pins;
  I2C_TypeDef *reg;
  i2c_mode_e mode; 
  hal_nvic_line_device_e irq;

  bool send;

  hal_i2c_data_s data;
} hal_i2c_config_s;

static hal_i2c_config_s i2c_pin_map[I2C_CHANNEL_COUNT] = {
  [I2C_CHANNEL_IDX(I2C(1))] = (hal_i2c_config_s){ 
    .pins = { 
      .scl = PORT_PIN('B', 6),
      .sda = PORT_PIN('B', 7) 
    },
    .reg = I2C1,
    .irq = NVIC_I2C1_EV
  },
  [I2C_CHANNEL_IDX(I2C(2))] = (hal_i2c_config_s){ 
    .pins = { 
      .scl = PORT_PIN('B', 10),
      .sda = PORT_PIN('B', 11)
    },
    .reg = I2C2,
    .irq = NVIC_I2C2_EV
  },
};

static inline void hal_i2c_common_isr(i2c_channel_t channel);


int hal_i2c_init(i2c_channel_t channel, i2c_mode_e mode, uint16_t address) {
  if (I2C_CHANNEL_IDX(channel) >= I2C_CHANNEL_COUNT) return -EINVAL;

  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  // Enable I2C clock
  RCC->APB1ENR |= (channel == I2C(1) ? RCC_APB1ENR_I2C1EN : RCC_APB1ENR_I2C2EN);

  hal_gpio_init();
  hal_gpio_configure(&cfg->pins.scl, GPIO_OUT_ALT_OPEN_DRAIN);
  hal_gpio_configure(&cfg->pins.sda, GPIO_OUT_ALT_OPEN_DRAIN);

  // Set I2C clock frequency to 10 MHz
  cfg->reg->CR2 |= (I2C_CR2_FREQ_3 | I2C_CR2_FREQ_1);
  // Set rise time based on the 10 MHz frequency
  cfg->reg->TRISE = 0x4;

  // Set to I2C mode
  cfg->reg->CR1 &= ~(I2C_CR1_SMBUS);

  switch (mode) {
    case I2C_SLAVE:
      // Set 7-bit address
      cfg->reg->OAR1 &= ~(I2C_OAR1_ADDMODE);
      cfg->reg->OAR1 |= (address & 0x7F) << I2C_OAR1_ADD1_Pos;

      // Bit that must be set
      cfg->reg->OAR1 |= (0x1 << 14);

      cfg->mode = I2C_SLAVE;
      break;

    case I2C_MASTER:
      // Configure clock using board parameters
      cfg->reg->CCR = CONFIG_STM32F1_I2C_CCR; 
      cfg->mode = I2C_MASTER;
      break;
  }

  // Enable interrupt
  cfg->reg->CR2 |= I2C_CR2_ITEVTEN;
  cfg->reg->CR2 |= I2C_CR2_ITBUFEN;
  hal_nvic_init_device(cfg->irq);

  // Enable I2C
  cfg->reg->CR1 |= I2C_CR1_PE;

  // ACK must be set after PE
  if (mode == I2C_SLAVE) {
    cfg->reg->CR1 |= I2C_CR1_ACK;
  }

  return 0;
}

const i2c_pins_s *hal_i2c_get_pins(i2c_channel_t channel) {
  if (I2C_CHANNEL_IDX(channel) >= I2C_CHANNEL_COUNT) return NULL;

  return &i2c_pin_map[I2C_CHANNEL_IDX(channel)].pins;
}

int hal_i2c_putn(i2c_channel_t channel, uint16_t slave_address, uint8_t *tx, uint16_t n) {
  if (I2C_CHANNEL_IDX(channel) >= I2C_CHANNEL_COUNT) return -EINVAL;

  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  // Previous transmission not yet completed
  if (cfg->data.i < cfg->data.n) return -EBUSY;

  cfg->data.tx = tx;
  cfg->data.i = 0;
  cfg->data.n = n;
  cfg->send = true;

  switch (cfg->mode) {
    case I2C_SLAVE:
      break;
    case I2C_MASTER: 
      cfg->data.target = (slave_address << 1u);

      // Send start condition
      cfg->reg->CR1 |= I2C_CR1_START;
      break;
  }

  return 0;
}

int hal_i2c_get(i2c_channel_t channel, uint8_t *rx) {
  if (I2C_CHANNEL_IDX(channel) >= I2C_CHANNEL_COUNT) return -EINVAL;

  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  return hal_rx_buffer_get(&cfg->data.rx_buf, rx);
}

static inline void hal_i2c_common_isr(i2c_channel_t channel) {
  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  // Start condition
  if (cfg->reg->SR1 & I2C_SR1_SB) {
    cfg->reg->DR = cfg->data.target;
  }

  // Address acknowledged
  else if (cfg->reg->SR1 & I2C_SR1_ADDR) { 
    // Master mode
    if (cfg->reg->SR2 & I2C_SR2_MSL) {
      // Send
      if (cfg->send) {
        cfg->reg->DR = cfg->data.tx[cfg->data.i++];
        cfg->send = false;
      }

      // Receive
      else {
        cfg->reg->CR1 |= I2C_CR1_ACK;
        // Generate NACK and STOP if only one byte is being transmitted
        if (cfg->data.n == 1) {
          cfg->reg->CR1 &= ~(I2C_CR1_ACK);
          cfg->reg->CR1 |= I2C_CR1_STOP;
        }
      }
    }
  }

  // Data register empty (transmitter)
  else if (cfg->reg->SR1 & I2C_SR1_TXE) {
    if (cfg->data.i < cfg->data.n) {
      cfg->reg->DR = cfg->data.tx[cfg->data.i++];
    } 
    else if (cfg->reg->SR1 & I2C_SR1_BTF) {
      cfg->reg->CR1 |= I2C_CR1_STOP;
    }
  }

  // Data register not empty (receiver)
  else if (cfg->reg->SR1 & I2C_SR1_RXNE) {
    hal_rx_buffer_put(&cfg->data.rx_buf, cfg->reg->DR);
    // Generate NACK and STOP after receiving the second to last byte
    if (cfg->mode == I2C_MASTER && (++cfg->data.i == cfg->data.n - 1)) {
      cfg->reg->CR1 &= ~(I2C_CR1_ACK);
      cfg->reg->CR1 |= I2C_CR1_STOP;
    }
  }

  // Slave must write to CR1 after STOP signal
  else if (cfg->reg->SR1 & I2C_SR1_STOPF) {
    cfg->reg->CR1 |= I2C_CR1_PE;
  }
}

void I2C1_EV_IRQHandler(void) {
  hal_i2c_common_isr(I2C(1));
}

void I2C2_EV_IRQHandler(void) {
  hal_i2c_common_isr(I2C(2));
}
