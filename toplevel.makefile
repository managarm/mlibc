
.DEFAULT_GOAL = all

PROTOC ?= protoc

include $(TREE_PATH)/rules.makefile
$(call include_dir,libc)
$(call include_dir,libm)

.PHONY: all clean gen install install-headers

all: all-libc all-libm

clean: clean-libc

gen: gen-libc

install: install-libc install-libm

install-headers: install-headers-libc

