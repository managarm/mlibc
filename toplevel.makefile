
.DEFAULT_GOAL = all

PROTOC ?= protoc

include $(TREE_PATH)/rules.makefile
include $(TREE_PATH)/libc.makefile
$(call include_dir,libm)
$(call include_dir,libpthread)

.PHONY: all clean gen install install-headers

all: libc.so all-libm all-libpthread

#clean: clean-libc

gen: gen-libc

install: install-libc install-libm install-libpthread

install-headers: install-headers-libc

