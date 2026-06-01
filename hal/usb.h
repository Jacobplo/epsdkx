#ifndef STM32F1XX_USB_H
#define STM32F1XX_USB_H

#include "stm32f103xb.h"

static inline void usb_init(void) {
  // Enable USB clock. Clock must be initialized properly first.
  RCC->APB1ENR |= RCC_APB1ENR_USBEN;
}

#endif
