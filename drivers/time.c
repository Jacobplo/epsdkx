#include <epsdkx/drivers/time.h>

#include <epsdkx/hal/time.h>

#include <stdint.h>


void time_init(void) {
  hal_time_init();
}

void time_delay_ticks(uint32_t ticks) {
  hal_time_delay_ticks(ticks);
}

uint32_t time_get_ticks(void) {
  return hal_time_get_ticks();
}

void time_delay_ms(uint32_t ms) {
  hal_time_delay_ms(ms);
}

