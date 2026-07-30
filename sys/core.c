#include <epsdkx/generated/config.h>

#include <stdint.h>

#include "stm32f1xx.h"

uint32_t SystemCoreClock;

void SystemCoreClockUpdate(void) {
  SystemCoreClock = CONFIG_CORE_CLOCK_FREQ;
}
