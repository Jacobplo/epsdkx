#include <epsdkx/hal/time.h>
#include "private/systick.h"

#include <stdint.h>


void hal_time_init(void) {
  hal_systick_init();
}

void hal_time_delay_ticks(uint32_t ticks) {
  hal_systick_delay_ticks(ticks);
}

uint32_t hal_time_get_ticks(void) {
  return hal_systick_get_ticks();
}

void hal_time_delay_ms(uint32_t ms) {
  hal_systick_delay_ms(ms);
}
