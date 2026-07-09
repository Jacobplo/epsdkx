#ifndef _EPSDKX_HAL_DMA_H
#define _EPSDKX_HAL_DMA_H

#include "stm32f1xx.h"

static inline void hal_dma_init(void) {
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}

#endif
