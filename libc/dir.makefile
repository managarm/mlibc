
$c_BINDIR := $(BUILD_PATH)/$c/bin

$c_LIBRARY_OBJS :=

$c_LDFLAGS := -nostdlib

$c_TARGETS := clean-$c gen-$c install-$c install-headers-$c $($c_BINDIR)/libc.so

.PHONY: all-$c install-$c install-headers-$c

all-$c: $($c_BINDIR)/libc.so

clean-$c:
	rm -f $($c_BINDIR)/libc.so

gen-$c: gen-$c/platform/x86_64-managarm

$(call include_dir,$c/generic/ansi)
$c_LIBRARY_OBJS += $($c/generic/ansi_OBJECT_PATHS)
clean-$c: clean-$c/generic/ansi
install-headers-$c: install-headers-$c/generic/ansi

$(call include_dir,$c/generic/posix)
$c_LIBRARY_OBJS += $($c/generic/posix_OBJECT_PATHS)
clean-$c: clean-$c/generic/posix
install-headers-$c: install-headers-$c/generic/posix

$(call include_dir,$c/generic/lsb)
$c_LIBRARY_OBJS += $($c/generic/lsb_OBJECT_PATHS)
clean-$c: clean-$c/generic/lsb
install-headers-$c: install-headers-$c/generic/lsb

$(call include_dir,$c/generic/linux)
$c_LIBRARY_OBJS += $($c/generic/linux_OBJECT_PATHS)
clean-$c: clean-$c/generic/linux
install-headers-$c: install-headers-$c/generic/linux

$(call include_dir,$c/compilers/gcc)
$c_LIBRARY_OBJS += $($c/compilers/gcc_OBJECT_PATHS)
clean-$c: clean-$c/compilers/gcc
install-headers-$c: install-headers-$c/compilers/gcc

$(call include_dir,$c/machine/x86_64)
$c_LIBRARY_OBJS += $($c/machine/x86_64_OBJECT_PATHS)
clean-$c: clean-$c/machine/x86_64
install-headers-$c: install-headers-$c/machine/x86_64

$(call include_dir,$c/platform/x86_64-managarm)
$c_LIBRARY_OBJS += $($c/platform/x86_64-managarm_OBJECT_PATHS)
all-$c: all-$c/platform/x86_64-managarm
clean-$c: clean-$c/platform/x86_64-managarm
install-$c: install-$c/platform/x86_64-managarm

$(call include_dir,$c/frigg-bindings)
$c_LIBRARY_OBJS += $($c/frigg-bindings_OBJECT_PATHS)
clean-$c: clean-$c/frigg-bindings

$c_BEGIN = $c/compilers/gcc/obj/mlibc_crtbegin.o
$c_END = $c/compilers/gcc/obj/mlibc_crtend.o

$c_LIBS := -l:ld-init.so

$($c_BINDIR):
	mkdir -p $@

$($c_BINDIR)/libc.so: $($c_LIBRARY_OBJS) $($c_BEGIN) $($c_END) | $($c_BINDIR)
	x86_64-managarm-g++ -shared -o $@ $($c_LDFLAGS) $($c_BEGIN) $($c_LIBRARY_OBJS) $($c_LIBS) $($c_END)

install-$c: install-headers-$c
	mkdir -p $(SYSROOT_PATH)/usr/lib
	install -p $($c_BINDIR)/libc.so $(SYSROOT_PATH)/usr/lib

