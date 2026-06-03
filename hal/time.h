#ifndef EPSDKX_HAL_TIME_H
#define EPSDKX_HAL_TIME_H

#include <stdint.h>

#include "common/status.h"

hal_status_e hal_time_init(void);

hal_status_e hal_time_delay_ticks(uint32_t ticks);

hal_status_e hal_time_get_ticks(uint32_t *ret_ticks);

hal_status_e hal_time_delay_ms(uint32_t ms);

#endif
