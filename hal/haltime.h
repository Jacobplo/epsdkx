#ifndef EPSDKX_HALTIME_H
#define EPSDKX_HALTIME_H

#include <stdint.h>

#include "common/halstatus.h"

hal_status_t haltime_init(void);

hal_status_t delay_ticks(uint32_t ticks);

hal_status_t get_ticks(uint32_t *ret_ticks);

hal_status_t delay_ms(uint32_t ms);

#endif
