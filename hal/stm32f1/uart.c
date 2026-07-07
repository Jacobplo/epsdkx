#include <epsdkx/hal/uart.h>

#include <epsdkx/hal/gpio.h>
#include <epsdkx/generated/config.h>
#include "private/dma.h"

#include <errno.h>
#include <stdint.h>

#include "stm32f103xb.h"
#include "stm32f1xx.h"


#ifndef CONFIG_GPIO
#error Must enable CONFIG_GPIO in order to use UART
#endif


#define UART_CONTROLLER_COUNT 3

typedef struct {
  hal_uart_pins_s pins;
  USART_TypeDef *reg;
} hal_uart_config_s;

static const hal_uart_config_s uart_pin_map[UART_CONTROLLER_COUNT] = {
  [UART(1)] = (hal_uart_config_s){ 
    .pins = { 
      .tx = PORT_PIN('A', 9),
      .rx = PORT_PIN('A', 10) 
    },
    .reg = USART1
  },
  [UART(2)] = (hal_uart_config_s){ 
    .pins = { 
      .tx = PORT_PIN('A', 2),
      .rx = PORT_PIN('A', 3)
    },
    .reg = USART2
  },
  [UART(3)] = (hal_uart_config_s){ 
    .pins = {
      .tx = PORT_PIN('B', 10),
      .rx = PORT_PIN('B', 11)
    },
    .reg = USART3
  },
};


static void hal_uart_set_baud_rate(hal_uart_controller_t controller, hal_uart_config_s cfg, uint32_t baud_rate);


int hal_uart_init(hal_uart_controller_t controller, uint32_t baud_rate) {
  if (controller >= UART_CONTROLLER_COUNT) return -EINVAL;

  hal_gpio_init();
  hal_dma_init();

  hal_uart_config_s cfg = uart_pin_map[controller]; 

  // Enable clock for UART
  switch(controller) {
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
  cfg.reg->CR1 |= USART_CR1_UE;

  // Select 8-bit word length
  cfg.reg->CR1 &= ~(USART_CR1_M);

  // Select 1 stop bit
  cfg.reg->CR2 &= ~(USART_CR2_STOP);

  // DMA enable transmitter and receiver
  // cfg.reg->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
  
  hal_uart_set_baud_rate(controller, cfg, baud_rate);
  
  return 0;
}

void hal_uart_set_baud_rate(hal_uart_controller_t controller, hal_uart_config_s cfg, uint32_t baud_rate) {
  uint32_t prescaler;
  uint8_t clock_divider;

  // Get PCLK
  if (controller == UART(1)) {
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

  cfg.reg->BRR = ((div_mantissa << USART_BRR_DIV_Mantissa_Pos) & USART_BRR_DIV_Mantissa_Msk) & 
                 ((div_fraction << USART_BRR_DIV_Fraction_Pos) & USART_BRR_DIV_Fraction_Msk);
}
