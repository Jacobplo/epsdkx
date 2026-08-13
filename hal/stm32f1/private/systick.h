#ifndef _EPSDKX_PLATFORM_SYSTICK_H
#define _EPSDKX_PLATFORM_SYSTICK_H

#include <stdint.h>


void hal_systick_init(void);

void hal_systick_delay_ticks(uint32_t ticks);

void hal_systick_delay_ms(uint32_t ms);

uint32_t hal_systick_get_ticks(void);

uint32_t hal_systick_ticks_to_ms(uint32_t ticks);

#endif
