/**
* Internal contract for busy-wait timming functions
*/

#ifndef _EPSDKX_HAL_TIME_H
#define _EPSDKX_HAL_TIME_H

#include <stdint.h>


/**
 * Should initialize the timing interrupt handler.
 */
void hal_time_init(void);

/**
 * Should use a while loop to delay execution for the provided number
 * of ticks, which are defined and tracked internally.
 */
void hal_time_delay_ticks(uint32_t ticks);

/**
 * Should return the current tick number. 
 */
uint32_t hal_time_get_ticks(void);

/**
 * Should call hal_time_delay_ticks() with the amount of ticks that would
 * take the provided number of milliseconds to complete.
 */
void hal_time_delay_ms(uint32_t ms);

#endif
