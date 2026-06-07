#include "epsdkx/hal/usb.h"
#include "epsdkx/hal/common/status.h"

#include "stm32f1xx.h"

hal_status_e usb_init(void) {
  // Enable USB clock. Clock must be initialized properly first.
  RCC->APB1ENR |= RCC_APB1ENR_USBEN;

  return HAL_STATUS_OK;
}
