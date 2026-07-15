EPSDKX_ROOT := $(shell git rev-parse --show-toplevel)

SOURCES := main.c
CFLAGS  := -Werror
INCLUDE := -isystem $(shell arm-none-eabi-gcc -print-sysroot)/include

-include $(EPSDKX_ROOT)/Makefile
