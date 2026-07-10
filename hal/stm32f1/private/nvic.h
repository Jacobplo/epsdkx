#ifndef _EPSDKX_HAL_NVIC_H
#define _EPSDKX_HAL_NVIC_H

#include "stm32f1xx.h"

typedef enum hal_nvic_line_device_e {
  NVIC_WWDG          = 0,
  NVIC_PVD           = 1,
  NVIC_TAMPER        = 2,
  NVIC_RTC           = 3,
  NVIC_FLASH         = 4,
  NVIC_RCC           = 5,
  NVIC_EXTI0         = 6,
  NVIC_EXTI1         = 7,
  NVIC_EXTI2         = 8,
  NVIC_EXTI3         = 9,
  NVIC_EXTI4         = 10,
  NVIC_DMA1_CHANNEL1 = 11,
  NVIC_DMA1_CHANNEL2 = 12,
  NVIC_DMA1_CHANNEL3 = 13,
  NVIC_DMA1_CHANNEL4 = 14,
  NVIC_DMA1_CHANNEL5 = 15,
  NVIC_DMA1_CHANNEL6 = 16,
  NVIC_DMA1_CHANNEL7 = 17,
  NVIC_ADC1_2        = 18,
  NVIC_CAN1_TX       = 19,
  NVIC_CAN1_RX0      = 20,
  NVIC_CAN1_RX1      = 21,
  NVIC_CAN1_SCE      = 22,
  NVIC_EXTI9_5       = 23,
  NVIC_TIM1_BRK      = 24,
  NVIC_TIM1_UP       = 25,
  NVIC_TIM1_TRG_COM  = 26,
  NVIC_TIM1_CC       = 27,
  NVIC_TIM2          = 28,
  NVIC_TIM3          = 29,
  NVIC_TIM4          = 30,
  NVIC_I2C1_EV       = 31,
  NVIC_I2C1_ER       = 32,
  NVIC_I2C2_EV       = 33,
  NVIC_I2C2_ER       = 34,
  NVIC_SPI1          = 35,
  NVIC_SPI2          = 36,
  NVIC_USART1        = 37,
  NVIC_USART2        = 38,
  NVIC_USART3        = 39,
  NVIC_EXTI15_10     = 40,
  NVIC_RTC_ALARM     = 41,
  NVIC_OTG_FS_WKUP   = 42,

  NVIC_TIM5          = 50,
  NVIC_SPI3          = 51,
  NVIC_UART4         = 52,
  NVIC_UART5         = 53,
  NVIC_TIM6          = 54,
  NVIC_TIM7          = 55,
  NVIC_DMA2_CHANNEL1 = 56,
  NVIC_DMA2_CHANNEL2 = 57,
  NVIC_DMA2_CHANNEL3 = 58,
  NVIC_DMA2_CHANNEL4 = 59,
  NVIC_DMA2_CHANNEL5 = 60,
  NVIC_ETH           = 61,
  NVIC_ETH_WKUP      = 62,
  NVIC_CAN2_TX       = 63,
  NVIC_CAN2_RX0      = 64,
  NVIC_CAN2_RX1      = 65,
  NVIC_CAN2_SCE      = 66,
  NVIC_OTG_FS        = 67
} hal_nvic_line_device_e;

static inline void hal_nvic_init_device(hal_nvic_line_device_e dev) {
  uint8_t iser_bit = 0x1 << (dev % 32u);

  if (dev < 32) {
    NVIC->ISER[0] |= iser_bit;
  } 
  else if (dev < 64) {
    NVIC->ISER[1] |= iser_bit;
  }
  else {
    NVIC->ISER[2] |= iser_bit;
  };
}

#endif
