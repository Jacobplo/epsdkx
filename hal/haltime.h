#ifndef EPSDKX_HALTIME_H
#define EPSDKX_HALTIME_H

#include <stdint.h>

#include "common/halstatus.h"

static inline hal_status_t haltime_init(void);

static inline hal_status_t delay_ticks(uint32_t ticks);

static inline hal_status_t get_ticks(uint32_t *ret);

#endif
