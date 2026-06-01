#ifndef STM32F1XX_SYSTICK_H
#define STM32F1XX_SYSTICK_H

#include <stdint.h>

#include "stm32f103xb.h"
#include "system_stm32f1xx.h"


static inline void systick_init(void) {
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);
}

void delay_ticks(uint32_t ticks);

uint32_t get_ticks(void);

#endif
