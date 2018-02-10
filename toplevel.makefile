
.DEFAULT_GOAL = all

PROTOC ?= protoc

include $(TREE_PATH)/rules.makefile
include $(TREE_PATH)/libc.makefile
$(call include_dir,libdl)
$(call include_dir,libm)
$(call include_dir,libpthread)
$(call include_dir,librt)
$(call include_dir,libutil)

.PHONY: all clean gen install install-headers

all: libc.so all-libdl all-libm all-libpthread all-librt all-libutil

#clean: clean-libc

gen: gen-libc

install: install-libc install-libdl install-libm install-libpthread install-librt install-libutil

install-headers: install-headers-libc

