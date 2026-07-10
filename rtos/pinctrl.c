#include <epsdkx/pinctrl.h>

#include <epsdkx/hal/pinctrl.h>


void pinctrl_init(void) {
  hal_pinctrl_init();
}

void pinctrl_disable_jtag(void) {
  hal_pinctrl_disable_jtag();
}
