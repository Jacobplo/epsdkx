# Application level Makefile
FAMILY 			:= stm32f1
BOARD   		:= bluepillplus
CC 					:= arm-none-eabi-gcc
OBJCOPY 		:= arm-none-eabi-objcopy

# sdk.mk
SDK_ROOT  	:= $(patsubst %/,%,$(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
BUILD_DIR 	:= build
OBJ_DIR   	:= $(BUILD_DIR)/objs
DEP_DIR   	:= $(BUILD_DIR)/deps
SOURCES			:= $(wildcard $(SDK_ROOT)/hal/platform/$(FAMILY)/*.c)
ASM_SOURCES :=
INCLUDE   	:= -I$(SDK_ROOT)
DEFINE			:=
CFLAGS  		:= -W -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion \
        		   -Wformat-truncation -fno-common \
        		   -g3 -Os -ffunction-sections -fdata-sections \
        		   -MMD -MP
LDFLAGS 		:= -lc -lgcc -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/firmware.map

# board.mk	 / family.mk
MCU_DIR 		:= $(SDK_ROOT)/hal/platform/$(FAMILY)/mcu/f103c8tx
DEFINE 			+= -DSTM32F103xB
SOURCES   	+= $(wildcard $(MCU_DIR)/*.c)
ASM_SOURCES	+= $(MCU_DIR)/startup.s
INCLUDE 		+= -I$(SDK_ROOT)/external/cmsis-device-f1/Include \
						 	 -I$(SDK_ROOT)/external/CMSIS_5/CMSIS/Core/Include
CFLAGS  		+= -mcpu=cortex-m3 -mthumb
LDFLAGS			+= -T$(MCU_DIR)/link.ld --specs=nano.specs

# sdk.mk
OBJS 				:= $(patsubst $(SDK_ROOT)/%,$(OBJ_DIR)/%.o,$(SOURCES:.c=))
ASM_OBJS    := $(patsubst $(SDK_ROOT)/%,$(OBJ_DIR)/%.o,$(ASM_SOURCES:.s=))
DEPS 				:= $(patsubst $(SDK_ROOT)/%,$(DEP_DIR)/%.d,$(SOURCES:.c=))

-include $(DEPS)



.PHONY: all
all: build

# Build object files and dependency files (.o and .d)
$(OBJ_DIR)/%.o: $(SDK_ROOT)/%.c	
	@printf '\tCC\t%s\n' $<
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*)
	@$(CC) $(CFLAGS) $(INCLUDE) $(DEFINE) -c $< -o $@ -MF $(DEP_DIR)/$*.d

# Build object files for ASM files
$(OBJ_DIR)/%.o: $(SDK_ROOT)/%.s
	@printf '\tCC\t%s\n' $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# Build flashable firmware
$(BUILD_DIR)/firmware.elf: $(OBJS) $(ASM_OBJS) $(MCU_DIR)/link.ld
	@printf '\tLD\t%s\n' $@
	@$(CC) $(OBJS) $(ASM_OBJS) $(CFLAGS) $(LDFLAGS) -o $@

$(BUILD_DIR)/firmware.bin: $(BUILD_DIR)/firmware.elf
	@printf '\tOBJCPY\t%s\n' $@
	@$(OBJCOPY) -O binary $< $@

.PHONY: build
build: $(BUILD_DIR)/firmware.bin

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
