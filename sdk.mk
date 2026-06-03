MCU_DIR = $(SDK_ROOT)/hal/platform/stm32f1/mcu/f103c8tx
FAMILY := stm32f1
BOARD := bluepillplus
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
DEFINE = -DSTM32F103xB

INCLUDE += -I$(SDK_ROOT)/external/cmsis-device-f1/Include -I$(SDK_ROOT)/external/CMSIS_5/CMSIS/Core/Include

SDK_ROOT  := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

# Add SDK root directory to the include list
INCLUDE   += -I$(SDK_ROOT)

SOURCES   = $(wildcard hal/platform/$(FAMILY)/*.c) $(MCU_DIR)/*.c

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
DEP_DIR   := $(BUILD_DIR)/deps

OBJ  			:= $(SOURCES:%.c=$(OBJ_DIR)/%.o)
DEPS 			:= $(SOURCES:%.c=$(DEP_DIR)/%.d)

CFLAGS  	?=  -W -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion \
        	    -Wformat-truncation -fno-common \
        	    -g3 -Os -ffunction-sections -fdata-sections -I. \
        	    -mcpu=cortex-m3 -mthumb -MMD -MP

LDFLAGS 	?= -T$(MCU_DIR)/link.ld --specs=nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map



.PHONY: default
all: build

# Build object files and dependency files (.o and .d)
$(OBJ_DIR)/%.o: %.c
	@printf '\tCC\t%s\n' $<
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*)
	@$(CC) $(CFLAGS) $(INCLUDE) $(DEFINE) -c $< -o $@ -MF $(DEP_DIR)/$*.d

$(OBJ_DIR)/startup.o: $(MCU_DIR)/startup.s
	@printf '\tCC\t%s\n' $<
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

# Build flashable firmware
$(BUILD_DIR)/firmware.elf: $(OBJ) $(OBJ_DIR)/startup.o $(MCU_DIR)/link.ld
	@printf '\tLD\t%s\n' $@
	@$(CC) $(OBJ) $(OBJ_DIR)/startup.o $(CFLAGS) $(LDFLAGS) $(INCLUDE) $(DEFINE) -o $@

$(BUILD_DIR)/firmware.bin: $(BUILD_DIR)/firmware.elf
	@printf '\tOBJCPY\t%s\n' $@
	@$(OBJCOPY) -O binary $< $@

.PHONY: build
build: $(BUILD_DIR)/firmware.bin

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
