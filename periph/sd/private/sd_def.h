#ifndef _EPSDKX_PERIPH_SD_DEF_H
#define _EPSDKX_PERIPH_SD_DEF_H

#include <stdint.h>


#define CMD(n) (n)

/**
 * SD SPI mode commands
 */
typedef enum sd_cmd_e {
  SD_CMD0  = CMD(0),  ///< GO_IDLE_STATE
  SD_CMD1  = CMD(1),  ///< SEND_OP_COND
  SD_CMD6  = CMD(6),  ///< SWITCH_FUNC
  SD_CMD8  = CMD(8),  ///< SEND_IF_COND
  SD_CMD9  = CMD(9),  ///< SEND_CSD
  SD_CMD10 = CMD(10), ///< SEND_CID
  SD_CMD12 = CMD(12), ///< STOP_TRANSMISSION
  SD_CMD13 = CMD(13), ///< SEND_STATUS
  SD_CMD16 = CMD(16), ///< SET_BLOCKLEN
  SD_CMD17 = CMD(17), ///< READ_SINGLE_BLOCK
  SD_CMD18 = CMD(18), ///< READ_MULTIPLE_BLOCK
  SD_CMD24 = CMD(24), ///< WRITE_BLOCK
  SD_CMD25 = CMD(25), ///< WRITE_MULTIPLE_BLOCK
  SD_CMD27 = CMD(27), ///< PROGRAM_CSD
  SD_CMD28 = CMD(28), ///< SET_WRITE_PROT
  SD_CMD29 = CMD(29), ///< CLR_WRITE_PROT
  SD_CMD30 = CMD(30), ///< SEND_WRITE_PROT
  SD_CMD32 = CMD(32), ///< ERASE_WR_BLK_START_ADDR
  SD_CMD33 = CMD(33), ///< ERASE_WR_BLK_END_ADDR
  SD_CMD38 = CMD(38), ///< ERASE
  SD_CMD42 = CMD(42), ///< LOCK_UNLOCK
  SD_CMD55 = CMD(55), ///< APP_CMD
  SD_CMD56 = CMD(56), ///< GEN_CMD
  SD_CMD58 = CMD(58), ///< READ_OCR
  SD_CMD59 = CMD(59), ///< CRC_ON_OFF
} sd_cmd_e;

/**
 * SD SPI mode responses
 */
typedef enum sd_r_e {
  SD_R1,
  SD_R1B,
  SD_R2,
  SD_R3,
  SD_R7
} sd_r_e;

#define CMD_FRAME_SIZE 6

typedef struct sd_command_frame_s {
  uint8_t bytes[CMD_FRAME_SIZE];
} sd_command_frame_s;

#define RESPONSE_FRAME_SIZE 5

typedef struct sd_response_frame_s {
  union {
    struct {
      uint8_t bytes[1];
      uint8_t reserved[4];
    } r1;
    struct {
      uint8_t bytes[2];
      uint8_t reserved[3];
    } r2;
    struct {
      uint8_t bytes[5];
    } r3;
    struct {
      uint8_t bytes[5];
    } r7;
  };
} sd_response_frame_s;

/*
 * Note that the following position refer to the position within each field's
 * respective byte.
 */
#define END_POS   0
#define CRC_POS   1
#define CMD_POS   0
#define TRAN_POS  6
#define START_POS 7
 
#define STATIC_CONSTRUCT_COMMAND(cmd, arg, crc)              \
  (sd_command_frame_s) {                                     \
    .bytes = {                                               \
      0x0 << START_POS | 0x1 << TRAN_POS | (cmd) << CMD_POS, \
      (arg) & 0xFF,                                          \
      ((arg) >> 8) & 0xFF,                                   \
      ((arg) >> 16) & 0xFF,                                  \
      ((arg) >> 24) & 0xFF,                                  \
      (crc) << CRC_POS | 0x1 << END_POS                      \
    }                                                        \
  }

#endif
