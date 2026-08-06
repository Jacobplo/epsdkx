#include "rcc.h"

#include <stdint.h>

#include "stm32f1xx.h"

uint32_t hal_rcc_get_pclk1(void) {
  extern uint32_t SystemCoreClock;
  uint32_t pclk1 = SystemCoreClock;

  uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;
  while (ppre1 > 3) {
    pclk1 /= 2;
    ppre1--;
  }

  return pclk1;
}

uint32_t hal_rcc_get_pclk2(void) {
  extern uint32_t SystemCoreClock;
  uint32_t pclk2 = SystemCoreClock;

  uint32_t ppre2 = (RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;
  while (ppre2 > 3) {
    pclk2 /= 2;
    ppre2--;
  }

  return pclk2;
}
