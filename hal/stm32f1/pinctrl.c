#include <epsdkx/hal/pinctrl.h>

#include "private/afio.h"


void hal_pinctrl_init(void) {
  hal_afio_init();
}

void hal_pinctrl_disable_jtag(void) {
  hal_afio_disable_jtag();
}
