#ifndef _EPSDKX_HAL_TIME_H
#define _EPSDKX_HAL_TIME_H

#include <stdint.h>


void hal_time_init(void);

void hal_time_delay_ticks(uint32_t ticks);

uint32_t hal_time_get_ticks(void);

void hal_time_delay_ms(uint32_t ms);

#endif
