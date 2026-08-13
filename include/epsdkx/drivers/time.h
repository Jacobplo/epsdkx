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
 * Delays program execution for a given number of milliseconds.
 */
void time_delay_ms(uint32_t ms);

/**
 * Returns the internal tick count.
 * 
 * Can be used alongside time_ticks_to_ms() to compute passed time.
 */
uint32_t time_get_ticks(void);


/**
 * Converts the provided tick count to its equivalent milliseconds.
 *
 * Can be used alongside time_get_ticks() to compute passed time.
 */
uint32_t time_ticks_to_ms(uint32_t ticks);

#endif
