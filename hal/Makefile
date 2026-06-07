-include $(HAL_DIR)/Makefile

HAL_MODULES  := GPIO UART USB

CHECK_KCONFIG += $(foreach module,$(HAL_MODULES), \
	if [ "$(CONFIG_$(module))" ] && [ "$(CONFIG_PLATFORM_SUPPORTS_$(module))" != "y" ]; then \
		echo "Error: Platform does not support $(module)"; \
		exit 1; \
	fi;)


ifeq ($(CONFIG_GPIO),y)
SOURCES += $(HAL_DIR)/gpio.c
endif

ifeq ($(CONFIG_UART),y)
SOURCES += $(HAL_DIR)/uart.c
endif

ifeq ($(CONFIG_USB),y)
SOURCES += $(HAL_DIR)/usb.c
endif
