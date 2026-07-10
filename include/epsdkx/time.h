#ifndef _EPSDKX_TIME_H
#define _EPSDKX_TIME_H

#include <stdint.h>


void time_init(void);

void time_delay_ticks(uint32_t ticks);

uint32_t time_get_ticks(void);

void time_delay_ms(uint32_t ms);

#endif
