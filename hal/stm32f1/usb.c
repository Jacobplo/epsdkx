#include <epsdkx/hal/usb.h>

#include "stm32f1xx.h"

void hal_usb_init(void) {
  // Enable USB clock. Clock must be initialized properly first.
  RCC->APB1ENR |= RCC_APB1ENR_USBEN;
}
