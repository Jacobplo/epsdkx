# Application level Makefile
FAMILY       := stm32f1
BOARD        := bluepillplus
CC           := arm-none-eabi-gcc
OBJCOPY      := arm-none-eabi-objcopy

# ==============
# Default Target
# ==============

.PHONY: all
all: setup check build

# ===================
# Directory Constants
# ===================

SDK_ROOT     := $(patsubst %/,%,$(dir $(realpath $(lastword $(MAKEFILE_LIST)))))
BOARD_DIR    := $(SDK_ROOT)/bsp/$(FAMILY)/$(BOARD)
HAL_DIR      := $(SDK_ROOT)/hal/$(FAMILY)
OUTPUT_DIR   := $(CURDIR)

BUILD_DIR    := $(OUTPUT_DIR)/build
OBJ_DIR      := $(BUILD_DIR)/objs
DEP_DIR      := $(BUILD_DIR)/deps
GEN_INC_DIR  := $(BUILD_DIR)/include/epsdkx/generated
STAMP_DIR    := $(BUILD_DIR)/stamps

# ================
# Build Parameters
# ================

INCLUDE      := -isystem $(SDK_ROOT)/include \
                -isystem $(BUILD_DIR)/include \
                -isystem $(BOARD_DIR)/include

DEFINE       :=

SOURCES      :=
ASM_SOURCES  :=
src-y        :=

CFLAGS       := -W -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion \
                -Wformat-truncation -fno-common -ffunction-sections -fdata-sections \
                -MMD -MP -O2

LDFLAGS      := -lc -lgcc -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/firmware.map

CHECK_KCONFIG :=

# ===================
# Kconfig Environment
# ===================

export KCONFIG_CONFIG     := $(BUILD_DIR)/.config
export KCONFIG_AUTOHEADER := $(GEN_INC_DIR)/config.h
export srctree            := $(OUTPUT_DIR)
export FAMILY             := $(FAMILY)

KCONFIGS := $(shell find $(SDK_ROOT) -name "Kconfig")

ifeq ($(filter clean, $(MAKECMDGOALS)),)
-include $(KCONFIG_CONFIG)
endif

ifeq ($(CONFIG_DEBUG),y)
CFLAGS += -g3
endif

# =================
# Makefile Includes
# =================

-include $(BOARD_DIR)/Makefile
-include $(SDK_ROOT)/hal/Makefile

# ============
# Object Files
# ============

SOURCES      += $(src-y)

OBJS         := $(SOURCES:%.c=$(OBJ_DIR)/%.o)
ASM_OBJS     := $(ASM_SOURCES:%.s=$(OBJ_DIR)/%.o)
DEPS         := $(SOURCES:%.c=$(DEP_DIR)/%.d)

# =======
# Targets
# =======

-include $(DEPS)

# Build object files and dependency files (.o and .d)
$(OBJ_DIR)/%.o: %.c $(KCONFIG_CONFIG)
	@printf '\tCC\t%s\n' $<
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*)
	@$(CC) $(CFLAGS) $(INCLUDE) $(DEFINE) -c $< -o $@ -MF $(DEP_DIR)/$*.d

# Build object files for ASM files
$(OBJ_DIR)/%.o: %.s $(KCONFIG_CONFIG)
	@printf '\tCC\t%s\n' $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# Build flashable firmware
$(BUILD_DIR)/firmware.elf: $(OBJS) $(ASM_OBJS) $(BOARD_DIR)/link.ld $(KCONFIG_CONFIG)
	@printf '\tLD\t%s\n' $@
	@$(CC) $(OBJS) $(ASM_OBJS) $(CFLAGS) $(LDFLAGS) -o $@

$(BUILD_DIR)/firmware.bin: $(BUILD_DIR)/firmware.elf
	@printf '\tOBJCPY\t%s\n' $@
	@$(OBJCOPY) -O binary $< $@



# Kconfig Targets

.PHONY: kconfig
kconfig: genconfig defconfig

.PHONY: genconfig
genconfig: $(KCONFIG_AUTOHEADER)

$(KCONFIG_AUTOHEADER): $(KCONFIG_CONFIG)
	@printf '\tKCONFIG\tgenconfig\n'
	@mkdir -p $(GEN_INC_DIR)
	@genconfig $(SDK_ROOT)/Kconfig

.PHONY: defconfig
defconfig: $(KCONFIG_CONFIG)

$(KCONFIG_CONFIG): $(KCONFIGS) $(srctree)/prj.conf
	@printf '\tKCONFIG\tdefconfig\n'
	@mkdir -p $(BUILD_DIR)
	@defconfig --kconfig $(SDK_ROOT)/Kconfig $(srctree)/prj.conf > /dev/null

.PHONY: check-kconfig
check-kconfig: $(STAMP_DIR)/check-kconfig.stamp

$(STAMP_DIR)/check-kconfig.stamp: $(KCONFIG_CONFIG)
	@mkdir -p $(dir $@)
	@printf '\tCHECK\tKconfig\n'
	@$(CHECK_KCONFIG)
	@touch $@



# Other Helper Targets

.PHONY: setup
setup: kconfig

.PHONY: check
check: setup check-kconfig

.PHONY: build
build: check $(BUILD_DIR)/firmware.bin

.PHONY: clean
clean:
	@printf '\tRM\t%s\n' build/
	@rm -rf $(BUILD_DIR)

.PHONY: compile_commands.json
compile_commands.json:
	@bear -- $(MAKE) -B build
