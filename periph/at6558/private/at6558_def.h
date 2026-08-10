#ifndef _EPSDKX_PERIPH_AT6558_DEF_H
#define _EPSDKX_PERIPH_AT6558_DEF_H

/**
 * CSIP message class
 */
typedef enum at6558_cisp_class_e {
  AT6558_NAV = 0x01,
  AT6558_TIM = 0x02,
  AT6558_RXM = 0x03,
  AT6558_ACK = 0x05,
  AT6558_CFG = 0x06,
  AT6558_MSG = 0x08,
  AT6558_MON = 0x0A,
  AT6558_AID = 0x0B,
} at6558_cisp_class_e;

/**
 * CSIP message id
 *
 * Only ids used in the driver need to be included.
 */
typedef enum at6558_cisp_id_e {
  AT6558_NAV_PV      = 0x03,
  AT6558_NAV_TIMEUTC = 0x10,
  AT6558_CFG_MSG     = 0x01,
  AT6558_CFG_RATE    = 0x04,
  AT6558_ACK_NACK    = 0x00,
  AT6558_ACK_ACK     = 0x01,
} at6558_cisp_id_e;

/**
 * The size of a full CSIP frame, including all fields, in bytes.
 */
typedef enum at6558_cisp_frame_len_e {
  AT6558_LEN_NAV_PV      = 90,
  AT6558_LEN_NAV_TIMEUTC = 34,
  AT6558_LEN_ACK         = 14,
} at6558_cisp_frame_len_e;

#define CSIP_HEADER      0xBA, 0xCE

#define BYTE_SPLIT(n,p)  ((uint8_t)(((n) >> (8 * (p))) & 0xFF))
#define BYTE_JOIN(n,p)   ((uint64_t)*((n) + (p)) << (8 * (p)))

#define SPLIT2(n) BYTE_SPLIT(n, 0), BYTE_SPLIT(n, 1)

#define SPLIT4(n)                                                              \
  BYTE_SPLIT(n, 0), BYTE_SPLIT(n, 1), BYTE_SPLIT(n, 2), BYTE_SPLIT(n, 3)

#define JOIN2(n) (BYTE_JOIN(n, 0) | BYTE_JOIN(n, 1))

#define JOIN4(n)                                                               \
  (BYTE_JOIN(n, 0) | BYTE_JOIN(n, 1) | BYTE_JOIN(n, 2) | BYTE_JOIN(n, 3))

#define JOIN8(n)                                                               \
  (BYTE_JOIN(n, 0) | BYTE_JOIN(n, 1) | BYTE_JOIN(n, 2) | BYTE_JOIN(n, 3) |     \
   BYTE_JOIN(n, 4) | BYTE_JOIN(n, 5) | BYTE_JOIN(n, 6) | BYTE_JOIN(n, 7))

#define CSIP_LEN(n)      SPLIT2(n)

#define CSIP_HEADER_POS  0
#define CSIP_LEN_POS     2
#define CSIP_CLASS_POS   4
#define CSIP_ID_POS      5
#define CSIP_PAYLOAD_POS 6

#define NAV_PV           AT6558_NAV, AT6558_NAV_PV
#define NAV_TIMEUTC      AT6558_NAV, AT6558_NAV_TIMEUTC
#define CFG_MSG          AT6558_CFG, AT6558_CFG_MSG
#define CFG_RATE         AT6558_CFG, AT6558_CFG_RATE


#endif
