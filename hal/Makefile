-include $(HAL_DIR)/Makefile

checkconfig:
		@if [ "$(CONFIG_GPIO)" ] && [ "$(CONFIG_PLATFORM_SUPPORTS_GPIO)" != "y" ]; then \
        echo "Error: Platform does not support GPIO"; \
				exit 1; \
		fi
		@if [ "$(CONFIG_UART)" ] && [ "$(CONFIG_PLATFORM_SUPPORTS_UART)" != "y" ]; then \
        echo "Error: Platform does not support UART"; \
				exit 1; \
		fi
		@if [ "$(CONFIG_USB)" ] && [ "$(CONFIG_PLATFORM_SUPPORTS_USB)" != "y" ]; then \
        echo "Error: Platform does not support USB"; \
				exit 1; \
		fi

ifeq ($(CONFIG_GPIO),y)
SOURCES += $(HAL_DIR)/gpio.c
endif

ifeq ($(CONFIG_UART),y)
SOURCES += $(HAL_DIR)/uart.c
endif

ifeq ($(CONFIG_USB),y)
SOURCES += $(HAL_DIR)/usb.c
endif
