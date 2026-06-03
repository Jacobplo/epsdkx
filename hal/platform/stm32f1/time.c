//#include "systick.h"
//
//static volatile uint32_t s_ticks;
//
//static void SysTick_Handler(void) {
//  s_ticks++;
//}
//
//void delay_ticks(uint32_t ticks) {
//  uint32_t start = s_ticks;
//  while(s_ticks < start + ticks) (void) 0;
//}
//
//uint32_t get_ticks(void) {
//  return s_ticks;
//}
