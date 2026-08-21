#include "diskio.h"
#include "ff.h"
#include "ffconf.h"
#include <epsdkx/periph/sd.h>
#include <epsdkx/periph/sd_fatfs_bridge.h>

#include <epsdkx/drivers/spi.h>
#include <epsdkx/common/spi.h>
#include <epsdkx/common/gpio.h>
#include <epsdkx/generated/config.h>

#include <stdbool.h>
#include <errno.h>


#if !defined(CONFIG_PERIPH_SD)
#error CONFIG_SD must be enabled to use the SD FatFs filesystem
#endif


typedef struct fatfs_drv_cfg_s {
  spi_channel_t channel;
  gpio_pin_u cs;
  bool configured;
  bool initialized;
} fatfs_drv_cfg_s;

static fatfs_drv_cfg_s cfgs[FF_VOLUMES];
static sd_dev_s devs[FF_VOLUMES];


int sd_fatfs_bridge_configure(BYTE pdrv, spi_channel_t channel, gpio_pin_u cs) {
  if (pdrv >= FF_VOLUMES) return -EINVAL;

  cfgs[pdrv].channel = channel;
  cfgs[pdrv].cs = cs;
  cfgs[pdrv].configured = true;
  cfgs[pdrv].initialized = false;

  return 0;
}

DSTATUS disk_status(BYTE pdrv) {
  DSTATUS ret = 0x00;

  if (!cfgs[pdrv].initialized) {
    ret |= STA_NOINIT;
  }

  return ret;
}

DSTATUS disk_initialize(BYTE pdrv) {
  if (pdrv >= FF_VOLUMES) return STA_NOINIT;

  int result = 0;

  fatfs_drv_cfg_s *cfg = &cfgs[pdrv];
  sd_dev_s *dev = &devs[pdrv];

  if (!cfg->configured) {
    result = -EINVAL;
  }

  if (result >= 0) {
    result = sd_init(dev, cfg->channel, cfg->cs);
  }

  if (result >= 0) {
    cfg->initialized = true;
  }

  return disk_status(pdrv);
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
  if (pdrv >= FF_VOLUMES) return RES_PARERR;

  DRESULT ret = RES_OK;

  sd_dev_s *dev = &devs[pdrv];

  if (disk_status(pdrv) & STA_NOINIT) {
    ret = RES_NOTRDY;
  }

  if (ret == RES_OK) {
    switch (cmd) {
      case CTRL_SYNC:
        // Do nothing, because the SD driver waits for write completion.
        break;

      case GET_SECTOR_COUNT:
        *(LBA_t *)buff = dev->prop.sector_count;
        break;

      case GET_SECTOR_SIZE:
        *(WORD *)buff = dev->prop.sector_size;
        break;

      case GET_BLOCK_SIZE:
        *(DWORD *)buff = dev->prop.erase_sector_size;
        break;

      case CTRL_TRIM:
        // Do nothing, because it is not supported.
        break;

      default:
        ret = RES_PARERR;
        break;
    }
  }

  return ret;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
  if (pdrv >= FF_VOLUMES) return RES_PARERR;

  DRESULT ret = RES_OK;
  int result;
  
  sd_dev_s *dev = &devs[pdrv];  

  if (disk_status(pdrv) & STA_NOINIT) {
    ret = RES_NOTRDY;
  } 

  if (ret == RES_OK) {
    // Check the provided sector and count work
    LBA_t sector_count;
    disk_ioctl(pdrv, GET_SECTOR_COUNT, &sector_count);
    if (sector + count > sector_count) {
      ret = RES_PARERR;
    }
  }

  if (ret == RES_OK) {
    if (count == 0) {
      ret = RES_PARERR;
    }
    else if (count == 1) {
      result = sd_read_block(dev, sector, buff);

      if (result < 0) {
        ret = RES_ERROR;
      }
    }
    else {
      result = sd_readn_block(dev, sector, buff, count);

      if (result < 0) {
        ret = RES_ERROR;
      }
    }
  }


  return ret;
}

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
  if (pdrv >= FF_VOLUMES) return RES_PARERR;

  DRESULT ret = RES_OK;
  int result;
  
  sd_dev_s *dev = &devs[pdrv];  

  if (disk_status(pdrv) & STA_NOINIT) {
    ret = RES_NOTRDY;
  } 

  if (ret == RES_OK) {
    // Check the provided sector and count work
    LBA_t sector_count;
    disk_ioctl(pdrv, GET_SECTOR_COUNT, &sector_count);
    if (sector + count > sector_count) {
      ret = RES_PARERR;
    }
  }

  if (ret == RES_OK) {
    if (count == 0) {
      ret = RES_PARERR;
    }
    else if (count == 1) {
      result = sd_write_block(dev, sector, buff);

      if (result < 0) {
        ret = RES_ERROR;
      }
    }
    else {
      result = sd_writen_block(dev, sector, buff, count);

      if (result < 0) {
        ret = RES_ERROR;
      }
    }
  }


  return ret;
}

#endif
