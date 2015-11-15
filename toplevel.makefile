
.DEFAULT_GOAL = all

include $(TREE_PATH)/rules.makefile
$(call include_dir,libc)

.PHONY: all clean gen

all: all-libc

clean: clean-libc

