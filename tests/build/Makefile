EPSDKX_ROOT := $(shell git rev-parse --show-toplevel)

-include $(EPSDKX_ROOT)/Makefile

SOURCES += main.c
CFLAGS  += -Werror
INCLUDE += -isystem $(shell arm-none-eabi-gcc -print-sysroot)/include
