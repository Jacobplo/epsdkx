#include "systick.h"

#include "stm32f1xx.h"

#include <stdint.h>


#define TICKS_PER_MS 1

static volatile uint32_t s_ticks;


void hal_systick_init(void) {
  extern uint32_t SystemCoreClock;
  SysTick_Config(SystemCoreClock / 1000);
}

void hal_systick_delay_ticks(uint32_t ticks) {
  uint32_t start = s_ticks;
  while((uint32_t)(s_ticks - start) < ticks) (void)0;
}

void hal_systick_delay_ms(uint32_t ms) {
  hal_systick_delay_ticks(ms * TICKS_PER_MS);
}

uint32_t hal_systick_get_ticks(void) {
  return s_ticks;
}

uint32_t hal_systick_ticks_to_ms(uint32_t ticks) {
  return ticks / TICKS_PER_MS;
}

void SysTick_Handler(void) {
  s_ticks++;
}
