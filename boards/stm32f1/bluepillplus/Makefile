DEFINE       += -DSTM32F103xB

src-y        += $(BOARD_DIR)/board.c
ASM_SOURCES  += $(BOARD_DIR)/startup.s

CFLAGS       += -mcpu=cortex-m3 -mthumb

LDFLAGS      += -T$(BOARD_DIR)/link.ld
