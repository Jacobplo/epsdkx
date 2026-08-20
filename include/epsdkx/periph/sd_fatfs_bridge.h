#include <epsdkx/periph/sd.h>
#include "diskio.h"

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
