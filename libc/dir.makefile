
$c_BINDIR := $(BUILD_PATH)/$c/bin

$c_LIBRARY_OBJS :=

$c_LDFLAGS := -nostdlib

$c_TARGETS := clean-$c gen-$c install-$c $($c_BINDIR)/libc.so

.PHONY: all-$c install-$c

all-$c: $($c_BINDIR)/libc.so

clean-$c:
	rm -f $($c_BINDIR)/libc.so

$(call include_dir,$c/generic/ansi)
$c_LIBRARY_OBJS += $($c/generic/ansi_OBJECT_PATHS)
clean-$c: clean-$c/generic/ansi
install-$c: install-$c/generic/ansi

$(call include_dir,$c/generic/posix)
$c_LIBRARY_OBJS += $($c/generic/posix_OBJECT_PATHS)
clean-$c: clean-$c/generic/posix
install-$c: install-$c/generic/posix

$(call include_dir,$c/generic/lsb)
$c_LIBRARY_OBJS += $($c/generic/lsb_OBJECT_PATHS)
clean-$c: clean-$c/generic/lsb
install-$c: install-$c/generic/lsb

$(call include_dir,$c/compilers/gcc)
install-$c: install-$c/compilers/gcc

$(call include_dir,$c/platform/x86_64-managarm)
$c_LIBRARY_OBJS += $($c/platform/x86_64-managarm_OBJECT_PATHS)
all-$c: all-$c/platform/x86_64-managarm
clean-$c: clean-$c/platform/x86_64-managarm
install-$c: install-$c/platform/x86_64-managarm

$(call include_dir,$c/frigg-bindings)
$c_LIBRARY_OBJS += $($c/frigg-bindings_OBJECT_PATHS)
clean-$c: clean-$c/frigg-bindings

$($c_BINDIR):
	mkdir -p $@

$($c_BINDIR)/libc.so: $($c_LIBRARY_OBJS) | $($c_BINDIR)
	x86_64-managarm-g++ -shared -o $@ $($c_LDFLAGS) $($c_LIBRARY_OBJS)

install-$c:
	mkdir -p $(SYSROOT_PATH)/usr/lib
	install $($c_BINDIR)/libc.so $(SYSROOT_PATH)/usr/lib

