#ifndef EPSDKX_HALSTATUS_H
#define EPSDKX_HALSTATUS_H

typedef enum {
  HAL_BUSY = 2,
  HAL_PENDING = 1,

  HAL_OK = 0,

  HAL_UNSUPPORTED = -1,
  HAL_UNIMPLEMENTED = -2,

  HAL_ERR_INVALID_ARG = -10,
  HAL_ERR_TIMEOUT     = -11,
  HAL_ERR_FAULT       = -12,
  HAL_ERR_OVERFLOW    = -13,
} hal_status_t;

#endif
