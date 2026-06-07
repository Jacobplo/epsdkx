#include <stdint.h>

#include "stm32f1xx.h"
#include "system_stm32f1xx.h"

uint32_t SystemCoreClock;

// Called automatically by startup code.
void SystemInit(void) {
  // Set 8 MHz HSE clock on.
  RCC->CR |= RCC_CR_HSEON;
  while(!(RCC->CR & RCC_CR_HSERDY)) (void) 0;

  // Reconfigure flash wait state for 72 MHz
  FLASH->ACR |= FLASH_ACR_PRFTBE;
  FLASH->ACR &= ~(FLASH_ACR_LATENCY);
  FLASH->ACR |= FLASH_ACR_LATENCY_2;

  // Set PLL clock source to HSE (8 MHz)
  RCC->CFGR |= RCC_CFGR_PLLSRC;

  // Multiply PLL by 9 (72 MHz)
  RCC->CFGR &= ~(RCC_CFGR_PLLMULL);
  RCC->CFGR |= (RCC_CFGR_PLLMULL9);

  // USB prescale PLL / 1.5 (48 MHz)
  RCC->CFGR &= ~(RCC_CFGR_USBPRE);

  // APB low speed prescaler of HCLK / 2 (36 MHz) [Cannot exceed 36 MHz]
  RCC->CFGR &= ~(RCC_CFGR_PPRE1);
  RCC->CFGR |= (RCC_CFGR_PPRE1_DIV2);
 
  // Enable PLL clock
  RCC->CR |= RCC_CR_PLLON;
  while(!(RCC->CR & RCC_CR_PLLRDY)) (void) 0;

  // Set SYSCLK to use PLL
  RCC->CFGR &= ~(RCC_CFGR_SW);
  RCC->CFGR |= (RCC_CFGR_SW_PLL);
}


void SystemCoreClockUpdate(void) {
  SystemCoreClock = 72000000;
}
