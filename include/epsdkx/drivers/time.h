/**
 * Public API for busy-wait timing functions.
 */

#ifndef _EPSDKX_TIME_H
#define _EPSDKX_TIME_H

#include <stdint.h>


/**
 * Initializes the busy-wait ticking interrupt handler.
 */
void time_init(void);

/**
 * Delays program execution for a given number of internally defined ticks.
 *
 * Intended to be used by other timing functions. Other functions like 
 * time_delay_ms() should be used in user-level code.
 */
void time_delay_ticks(uint32_t ticks);

/**
 * Returns the internal tick count.
 *
 * Currently has no practical use, but is intended for use by 
 * yet-to-be-implemented functions for measuring execution time.
 */
uint32_t time_get_ticks(void);

/**
 * Delays program execution for a given number of milliseconds.
 */
void time_delay_ms(uint32_t ms);

#endif
