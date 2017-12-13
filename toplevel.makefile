
.DEFAULT_GOAL = all

PROTOC ?= protoc

include $(TREE_PATH)/rules.makefile
$(call include_dir,libc)
$(call include_dir,libm)
$(call include_dir,libpthread)

.PHONY: all clean gen install install-headers

all: all-libc all-libm all-libpthread

clean: clean-libc

gen: gen-libc

install: install-libc install-libm install-libpthread

install-headers: install-headers-libc

