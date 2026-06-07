-include $(HAL_DIR)/Makefile

HAL_MODULES  := GPIO UART USB

CHECK_KCONFIG += $(foreach module,$(HAL_MODULES), \
	if [ "$(CONFIG_$(module))" ] && [ "$(CONFIG_PLATFORM_SUPPORTS_$(module))" != "y" ]; then \
		echo "Error: Platform does not support $(module)"; \
		exit 1; \
	fi;)

src-$(CONFIG_GPIO) += $(HAL_DIR)/gpio.c
src-$(CONFIG_UART) += $(HAL_DIR)/uart.c
src-$(CONFIG_USB)  += $(HAL_DIR)/usb.c
