#ifndef STM32F103_BLUE_PILL_PLUS_H
#define STM32F103_BLUE_PILL_PLUS_H

#include "gpio.h"

#define PIN_LED             PIN('B', 2)

#define PIN_BTN             PIN('A', 0)

#define PIN_USB_DN          PIN('A', 11)
#define PIN_USB_DP          PIN('A', 12)

#define PIN_SWDIO           PIN('A', 13)
#define PIN_SWCLK           PIN('A', 14)

#define PIN_SPI_FLASH_CS    PIN('A', 4)
#define PIN_SPI_FLASH_MISO  PIN('A', 6)
#define PIN_SPI_FLASH_MOSI  PIN('A', 7)
#define PIN_SPI_FLASH_SCK   PIN('A', 5)

#endif
