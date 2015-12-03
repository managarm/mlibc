
.DEFAULT_GOAL = all

include $(TREE_PATH)/rules.makefile
$(call include_dir,libc)
$(call include_dir,libm)

.PHONY: all clean gen install

all: all-libc all-libm

clean: clean-libc

install: install-libc install-libm

