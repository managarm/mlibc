
.DEFAULT_GOAL = all

PROTOC ?= protoc

include $(TREE_PATH)/rules.makefile
include $(TREE_PATH)/libc.makefile
$(call include_dir,libdl)
$(call include_dir,libm)
$(call include_dir,libpthread)
$(call include_dir,librt)

.PHONY: all clean gen install install-headers

all: libc.so all-libdl all-libm all-libpthread all-librt

#clean: clean-libc

gen: gen-libc

install: install-libc install-libdl install-libm install-libpthread install-librt

install-headers: install-headers-libc

