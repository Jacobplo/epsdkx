#include <epsdkx/common/i2c.h>
#include <epsdkx/hal/i2c.h>

#include <epsdkx/hal/gpio.h>
#include <epsdkx/hal/rx_buffer.h>
#include <epsdkx/generated/config.h>
#include "private/nvic.h"
#include "private/rcc.h"

#include <epsdkx/hal/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <stddef.h>

#include "stm32f1xx.h"


#ifndef CONFIG_GPIO
#error Must enable CONFIG_GPIO to use GPIO
#endif

#define I2C_CHANNEL_IDX(n) ((n) - 1)
#define I2C_CHANNEL_COUNT  2

typedef struct hal_i2c_data_s{
  uint16_t target;
  uint8_t *tx;
  volatile uint16_t i;
  uint16_t n;
  volatile bool send;
  volatile bool busy;

  hal_rx_buffer_s rx_buf; 
} hal_i2c_data_s;

typedef struct hal_i2c_config_s {
  i2c_pins_s pins;
  I2C_TypeDef *reg;
  i2c_mode_e mode; 
  hal_nvic_line_device_e event_irq;
  hal_nvic_line_device_e error_irq;

  hal_i2c_data_s data;
} hal_i2c_config_s;

static hal_i2c_config_s i2c_pin_map[I2C_CHANNEL_COUNT] = {
  [I2C_CHANNEL_IDX(I2C(1))] = (hal_i2c_config_s){ 
    .pins = { 
      .scl = PORT_PIN('B', 6),
      .sda = PORT_PIN('B', 7) 
    },
    .reg = I2C1,
    .event_irq = NVIC_I2C1_EV,
    .error_irq = NVIC_I2C1_ER
  },
  [I2C_CHANNEL_IDX(I2C(2))] = (hal_i2c_config_s){ 
    .pins = { 
      .scl = PORT_PIN('B', 10),
      .sda = PORT_PIN('B', 11)
    },
    .reg = I2C2,
    .event_irq = NVIC_I2C2_EV,
    .error_irq = NVIC_I2C2_ER
  },
};

static void hal_i2c_set_timings(i2c_channel_t channel);
static inline void hal_i2c_event_isr(i2c_channel_t channel);
static inline void hal_i2c_error_isr(i2c_channel_t channel);


int hal_i2c_init(i2c_channel_t channel, i2c_mode_e mode, uint16_t address) {
  if (I2C_CHANNEL_IDX(channel) >= I2C_CHANNEL_COUNT) return -EINVAL;

  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  // Enable I2C clock
  RCC->APB1ENR |= (channel == I2C(1) ? RCC_APB1ENR_I2C1EN : RCC_APB1ENR_I2C2EN);

  hal_gpio_init();
  hal_gpio_configure(&cfg->pins.scl, GPIO_OUT_ALT_OPEN_DRAIN);
  hal_gpio_configure(&cfg->pins.sda, GPIO_OUT_ALT_OPEN_DRAIN); 

  // Set to SMBus mode
  cfg->reg->CR1 |= I2C_CR1_SMBUS;

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
      cfg->mode = I2C_MASTER;
      break;
  }

  // Called after mode is set.
  hal_i2c_set_timings(channel);

  // Enable interrupt
  cfg->reg->CR2 |= I2C_CR2_ITEVTEN;
  cfg->reg->CR2 |= I2C_CR2_ITBUFEN;
  cfg->reg->CR2 |= I2C_CR2_ITERREN;
  hal_nvic_init_device(cfg->event_irq);
  hal_nvic_init_device(cfg->error_irq);

  // Enable I2C
  cfg->reg->CR1 |= I2C_CR1_PE; 

  return 0;
}

const i2c_pins_s *hal_i2c_get_pins(i2c_channel_t channel) {
  if (I2C_CHANNEL_IDX(channel) >= I2C_CHANNEL_COUNT) return NULL;

  return &i2c_pin_map[I2C_CHANNEL_IDX(channel)].pins;
}

bool hal_i2c_is_busy(i2c_channel_t channel) {
  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  return (cfg->reg->SR2 & I2C_SR2_BUSY) || cfg->data.busy;
}

int hal_i2c_writen(i2c_channel_t channel, uint8_t *tx, uint16_t n, uint16_t slave_address) {
  if (I2C_CHANNEL_IDX(channel) >= I2C_CHANNEL_COUNT) return -EINVAL;

  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  // Previous transmission not yet completed
  if (hal_i2c_is_busy(channel)) return -EBUSY;

  cfg->data.tx = tx;
  cfg->data.i = 0;
  cfg->data.n = n;
  cfg->data.send = true;
  cfg->data.busy = true;

  if (cfg->mode == I2C_MASTER) {
    cfg->data.target = (slave_address << 1u);
    cfg->reg->CR1 |= I2C_CR1_START;
  }
  else {
    cfg->reg->CR1 |= I2C_CR1_ACK;
  }

  return 0;
}

int hal_i2c_readn(i2c_channel_t channel, uint16_t n, uint16_t slave_address) {
  if (I2C_CHANNEL_IDX(channel) >= I2C_CHANNEL_COUNT) return -EINVAL;

  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  // Previous transmission not yet completed
  if (hal_i2c_is_busy(channel)) return -EBUSY;

  cfg->data.i = 0;
  cfg->data.n = n;
  cfg->data.send = false;
  cfg->data.busy = true;

  if (cfg->mode == I2C_MASTER) {
    cfg->data.target = (slave_address << 1u) | 0x1;
    cfg->reg->CR1 |= I2C_CR1_START;
  }
  else {
    cfg->reg->CR1 |= I2C_CR1_ACK;
  }

  return 0;
}

int hal_i2c_get(i2c_channel_t channel, uint8_t *rx) {
  if (I2C_CHANNEL_IDX(channel) >= I2C_CHANNEL_COUNT) return -EINVAL;

  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  return hal_rx_buffer_get(&cfg->data.rx_buf, rx);
}

static void hal_i2c_set_timings(i2c_channel_t channel) {
  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  uint32_t pclk = hal_rcc_get_pclk1();
  uint16_t freq_mhz = pclk / 1000000;

  // Set I2C clock frequency to PCLK frequency
  cfg->reg->CR2 = freq_mhz;

  // Set rise time based pclk frequency and standard mode max rise time of 1000 ns
  cfg->reg->TRISE = freq_mhz + 1;

  if (cfg->mode == I2C_MASTER) {
    // Width of half a period at 100 kHz is 5000 ns, so CCR must multiply with
    // PCLK period to get 5000 ns
    cfg->reg->CCR = 5 * freq_mhz;
  }
}

static inline void hal_i2c_event_isr(i2c_channel_t channel) {
  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  // Start condition
  if (cfg->reg->SR1 & I2C_SR1_SB) {
    cfg->reg->DR = cfg->data.target;
  }

  // Address acknowledged
  else if (cfg->reg->SR1 & I2C_SR1_ADDR) { 
    // Receiver (master)
    if (cfg->reg->SR2 & I2C_SR2_MSL && !cfg->data.send) { 
      cfg->reg->CR1 |= I2C_CR1_ACK;
      // Generate NACK and STOP if only one byte is being transmitted
      if (cfg->data.n == 1) {
        cfg->reg->CR1 &= ~(I2C_CR1_ACK);
        cfg->reg->CR1 |= I2C_CR1_STOP;
        cfg->data.busy = false;
      }
    }
    // Transmitter (master / slave)
    else {
      cfg->reg->DR = cfg->data.tx[cfg->data.i++];
      cfg->data.send = false;
    }
  }

  // Data register empty (transmitter)
  else if (cfg->reg->SR1 & I2C_SR1_TXE) {
    // Transmit data (master / slave)
    if (cfg->data.i < cfg->data.n) {
      cfg->reg->DR = cfg->data.tx[cfg->data.i++];
    } 
    // Send STOP signal (master)
    else if (cfg->mode == I2C_MASTER && cfg->reg->SR1 & I2C_SR1_BTF) {
      cfg->reg->CR1 |= I2C_CR1_STOP;
      cfg->data.busy = false;
    }
  }

  // Data register not empty (receiver)
  else if (cfg->reg->SR1 & I2C_SR1_RXNE) {
    // Receive data into RX buffer (master / slave)
    hal_rx_buffer_put(&cfg->data.rx_buf, cfg->reg->DR);

    // Generate NACK and STOP after receiving the second to last byte (master)
    if (cfg->mode == I2C_MASTER) {
      if (++cfg->data.i == cfg->data.n - 1) {
        cfg->reg->CR1 &= ~(I2C_CR1_ACK);
        cfg->reg->CR1 |= I2C_CR1_STOP;
      }
      else if (cfg->data.i == cfg->data.n) {
        cfg->data.busy = false;
      }
    }
  }

  // Slave must write to CR1 after STOP signal
  else if (cfg->reg->SR1 & I2C_SR1_STOPF) {
    cfg->reg->CR1 |= I2C_CR1_PE;
    cfg->data.busy = false;
  }
}

void I2C1_EV_IRQHandler(void) {
  hal_i2c_event_isr(I2C(1));
}

void I2C2_EV_IRQHandler(void) {
  hal_i2c_event_isr(I2C(2));
}

static inline void hal_i2c_error_isr(i2c_channel_t channel) {
  hal_i2c_config_s *cfg = &i2c_pin_map[I2C_CHANNEL_IDX(channel)];

  // Bus error (misplaced START or STOP condition)
  if (cfg->reg->SR1 & I2C_SR1_BERR) {
    cfg->reg->SR1 &= ~(I2C_SR1_BERR);
    cfg->data.busy = false;
  }

  // Arbitration lost (master)
  else if (cfg->reg->SR1 & I2C_SR1_ARLO) {
    cfg->reg->SR1 &= ~(I2C_SR1_ARLO);
    cfg->data.busy = false;
  }

  // Timeout
  else if (cfg->reg->SR1 & I2C_SR1_TIMEOUT) {
    cfg->reg->SR1 &= ~(I2C_SR1_TIMEOUT);
    cfg->data.busy = false;
  }

  // Acknowledge failure
  else if (cfg->reg->SR1 & I2C_SR1_AF) {
    cfg->reg->SR1 &= ~(I2C_SR1_AF);
    cfg->data.busy = false;
  }

  // Overrun / underrun
  else if (cfg->reg->SR1 & I2C_SR1_OVR) {
    cfg->reg->SR1 &= ~(I2C_SR1_OVR);
    cfg->data.busy = false;
  }

  // PEC error in reception
  else if (cfg->reg->SR1 & I2C_SR1_PECERR) {
    cfg->reg->SR1 &= ~(I2C_SR1_PECERR);
    cfg->data.busy = false;
  }
}

void I2C1_ER_IRQHandler(void) {
  hal_i2c_error_isr(I2C(1));
}

void I2C2_ER_IRQHandler(void) {
  hal_i2c_error_isr(I2C(2));
}
