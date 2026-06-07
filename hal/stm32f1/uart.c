#include <epsdkx/hal/uart.h>

#include "stm32f1xx.h"

void uart_init(void) {
  // USART1
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  // USART2-3
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN | RCC_APB1ENR_USART3EN;
}
