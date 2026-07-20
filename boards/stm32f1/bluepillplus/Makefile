INCLUDE      += -I$(SDK_ROOT)/external/cmsis-device-f1/Include \
                -I$(SDK_ROOT)/external/CMSIS_5/CMSIS/Core/Include

DEFINE       += -DSTM32F103xB

SOURCES      += $(BOARD_DIR)/board.c
ASM_SOURCES  += $(BOARD_DIR)/startup.s

CFLAGS       += -mcpu=cortex-m3 -mthumb

LDFLAGS      += -T$(BOARD_DIR)/link.ld
