#include <epsdkx/hal/i2c.h>

#include <epsdkx/hal/gpio.h>
#include <epsdkx/generated/config.h>
#include "private/nvic.h"
#include "private/rx_buffer.h"

#include <stdint.h>
#include <errno.h>
#include <stddef.h>

#include "stm32f1xx.h"


#ifndef CONFIG_GPIO
#error Must enable CONFIG_GPIO to use GPIO
#endif

#ifndef CONFIG_STM32F1_I2C_CCR
#error Must set CONFIG_STM32F1_I2C_CCR in the board Kconfig, as defined in the datasheet
#endif

#define I2C_CHANNEL_IDX(n) ((n) - 1)
#define I2C_CHANNEL_COUNT  2

typedef struct hal_i2c_config_s {
  i2c_pins_s pins;
  I2C_TypeDef *reg;
  i2c_mode_e mode;
  hal_nvic_line_device_e irq;
  hal_rx_buffer_s rx_buf;
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

  // Set to I2C mode
  cfg->reg->CR1 &= ~(I2C_CR1_SMBUS);

  switch (mode) {
    case I2C_SLAVE:
      // Set 7-bit address
      cfg->reg->OAR1 &= ~(I2C_OAR1_ADDMODE);
      cfg->reg->OAR1 |= (address & 0xEF) << I2C_OAR1_ADD1_Pos;
      break;

    case I2C_MASTER:
      // Configure clock using board parameters
      cfg->reg->CCR = CONFIG_STM32F1_I2C_CCR;

      // Set rise time based on the 10 MHz frequency
      cfg->reg->TRISE = 0x4;
      break;
  }

  // Enable interrupt
  cfg->reg->CR2 |= I2C_CR2_ITBUFEN;
  cfg->reg->CR2 |= I2C_CR2_ITEVTEN;
  hal_nvic_init_device(cfg->irq);

  // Enable I2C
  cfg->reg->CR1 |= I2C_CR1_PE;

  return 0;
}

const i2c_pins_s *hal_i2c_get_pins(i2c_channel_t channel) {
  if (I2C_CHANNEL_IDX(channel) >= I2C_CHANNEL_COUNT) return NULL;

  return &i2c_pin_map[I2C_CHANNEL_IDX(channel)].pins;
}

int hal_i2c_put(i2c_channel_t channel, uint8_t tx) {
  (void)channel;
  (void)tx;
  return 0;
}

int hal_i2c_get(i2c_channel_t channel, uint8_t *rx) {
  (void)channel;
  (void)rx;
  return 0;
}
