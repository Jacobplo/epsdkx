#ifndef __EPSDKX_HAL_PINCTRL_H
#define __EPSDKX_HAL_PINCTRL_H

#include <epsdkx/hal/common/status.h>

hal_status_e hal_pinctrl_init(void);

hal_status_e hal_pinctrl_disable_jtag(void);

#endif
