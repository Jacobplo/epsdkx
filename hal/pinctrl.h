#ifndef EPSDKX_PINCTRL_H
#define EPSDKX_PINCTRL_H

#include "common/halstatus.h"

hal_status_t pinctrl_init(void);

hal_status_t pinctrl_disable_jtag(void);

#endif
