#include <epsdkx/hal/dma.h>

#include "stm32f1xx.h"


void hal_dma_init(void) {
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}
