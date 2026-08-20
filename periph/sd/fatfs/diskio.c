#include "diskio.h"
#include "ff.h"

DSTATUS disk_status(BYTE pdrv) {
  DSTATUS stat;
  int result;

  (void)pdrv;
  (void)stat;
  (void)result;

  return STA_NOINIT;
}

DSTATUS disk_initialize(BYTE pdrv) {
  DSTATUS stat;
  int result;

  (void)pdrv;
  (void)stat;
  (void)result;

  return STA_NOINIT;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
  DRESULT res;
  int result;

  (void)pdrv;
  (void)buff;
  (void)sector;
  (void)count;
  (void)res;
  (void)result;

  return RES_PARERR;
}

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
  DRESULT res;
  int result;

  (void)pdrv;
  (void)buff;
  (void)sector;
  (void)count;
  (void)res;
  (void)result;

  return RES_PARERR;
}

#endif

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
  DRESULT res;
  int result;

  (void)pdrv;
  (void)cmd;
  (void)buff;
  (void)res;
  (void)result;

  return RES_PARERR;
}
