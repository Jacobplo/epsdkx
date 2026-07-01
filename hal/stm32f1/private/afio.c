#include "afio.h"

#include "stm32f1xx.h"


void hal_afio_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
}

void hal_afio_disable_jtag(void) {
  AFIO->MAPR &= ~(AFIO_MAPR_SWJ_CFG);
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}
