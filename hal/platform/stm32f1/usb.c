#include "hal/usb.h"
#include "hal/common/halstatus.h"

#include "stm32f1xx.h"

hal_status_t usb_init(void) {
  // Enable USB clock. Clock must be initialized properly first.
  RCC->APB1ENR |= RCC_APB1ENR_USBEN;

  return HAL_STATUS_OK;
}
