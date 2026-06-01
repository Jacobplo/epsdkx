#ifndef STM32F1XX_USART_H
#define STM32F1XX_USART_H

#include <stdint.h>

#include "stm32f103xb.h"


static inline void usart_init(void) {
  // USART1
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  // USART2-3
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN | RCC_APB1ENR_USART3EN;
}

#endif
