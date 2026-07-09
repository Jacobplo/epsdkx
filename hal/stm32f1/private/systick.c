#include "systick.h"

#include "stm32f1xx.h"

#include <stdint.h>


static volatile uint32_t s_ticks;


void hal_systick_init(void) {
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);
}

void hal_systick_delay_ticks(uint32_t ticks) {
  uint32_t start = s_ticks;
  while(s_ticks < start + ticks) (void)0;
}

uint32_t hal_systick_get_ticks(void) {
  return s_ticks;
}

void hal_systick_delay_ms(uint32_t ms) {
  hal_systick_delay_ticks(ms);
}

void SysTick_Handler(void) {
  s_ticks++;
}
