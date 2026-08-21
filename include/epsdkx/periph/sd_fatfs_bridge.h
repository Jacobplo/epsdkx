/**
 * This module enables compatibility between the epsdkx SD driver and FatFs.
 *
 * Note that a default ffconf.h is provided as part of this driver to ensure
 * proper compatibility, but CONFIG_PERIPH_SD_FATFS_FFCONF can be toggled off if
 * the user wants to provide their own ffconf.h.
 */

#ifndef _EPSDKX_PERIPH_SD_FATFS_BRIDGE_H
#define _EPSDKX_PERIPH_SD_FATFS_BRIDGE_H

#include <epsdkx/periph/sd.h>

#include <epsdkx/common/spi.h>
#include <epsdkx/common/gpio.h>
#include "ff.h"

/**
 * Bridges SD SPI channel and chips select configuration with the internal
 * logical drive mapping used in FatFs.
 *
 * Must be called before the FatFs f_mount() function.
 *
 * Returns a negative errno on failure, or 0 on success.
 */
int sd_fatfs_bridge_configure(BYTE pdrv, spi_channel_t channel, gpio_pin_u cs);

#endif
