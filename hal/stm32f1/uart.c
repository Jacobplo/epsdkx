#include "epsdkx/hal/uart.h"
#include "epsdkx/hal/common/status.h"

#include "stm32f1xx.h"

hal_status_e uart_init(void) {
  // USART1
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  // USART2-3
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN | RCC_APB1ENR_USART3EN;

  return HAL_STATUS_OK;
}
