#include <epsdkx/generated/config.h>

#include <stdint.h>

uint32_t SystemCoreClock;

void SystemCoreClockUpdate(void) {
  SystemCoreClock = CONFIG_CORE_CLOCK_FREQ;
}
