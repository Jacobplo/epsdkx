#ifndef STM32F1XX_AFIO_H
#define STM32F1XX_AFIO_H

#include "stm32f103xb.h"

static inline void afio_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
}

static inline void afio_disable_jtag(void) {
  AFIO->MAPR &= ~(AFIO_MAPR_SWJ_CFG);
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}

#endif
