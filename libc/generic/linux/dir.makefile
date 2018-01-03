
$c_SRCDIR = $(TREE_PATH)/$c/src
$c_HEADERDIR := $(TREE_PATH)/$c/include
$c_OBJDIR := $(BUILD_PATH)/$c/obj

$c_HEADERS := getopt.h mntent.h malloc.h poll.h \
	linux/filter.h linux/sched.h linux/input.h linux/types.h linux/netlink.h linux/magic.h \
	linux/sockios.h linux/bpf_common.h \
	stdio_ext.h sys/inotify.h sys/ioctl.h sys/mount.h sys/random.h \
	sys/sysmacros.h sys/sendfile.h

$c_OBJECTS := getopt-stubs.o mntent-stubs.o poll-stubs.o stdio_ext-stubs.o sys-inotify-stubs.o
$c_OBJECTS += sys-random-stubs.o sys-sendfile-stubs.o
$c_OBJECT_PATHS := $(addprefix $($c_OBJDIR)/,$($c_OBJECTS))

$c_CXX := x86_64-managarm-g++
$c_CPPFLAGS := -std=c++11 -Wall
$c_CPPFLAGS += -I$(TREE_PATH)/libc/compilers/gcc/private
$c_CPPFLAGS += -I$(FRIGG_PATH)/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/ansi/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/posix/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/lsb/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/linux/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/compilers/gcc/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/platform/x86_64-managarm/include
$c_CPPFLAGS += -DFRIGG_HAVE_LIBC -DFRIGG_HIDDEN
$c_CXXFLAGS := $($c_CPPFLAGS) -fPIC -O2
$c_CXXFLAGS += -fno-builtin -fno-rtti -fno-exceptions

$c_TARGETS := clean-$c install-headers-$c $($c_OBJECT_PATHS)

.PHONY: clean-$c install-headers-$c

clean-$c:
	rm -f $($c_OBJECT_PATHS)
	rm -f $($c_OBJECT_PATHS:%.o=%.d)

install-headers-$c:
	mkdir -p  $(SYSROOT_PATH)/usr/include
	mkdir -p  $(SYSROOT_PATH)/usr/include/linux
	mkdir -p  $(SYSROOT_PATH)/usr/include/sys
	mkdir -p  $(SYSROOT_PATH)/usr/include/mlibc
	for f in $($c_HEADERS); do \
		install $($c_HEADERDIR)/$$f $(SYSROOT_PATH)/usr/include/$$f; done

$($c_OBJDIR):
	mkdir -p $@

$($c_OBJDIR)/%.o: $($c_SRCDIR)/%.cpp | $($c_OBJDIR)
	$($c_CXX) -c -o $@ $($c_CXXFLAGS) $<
	$($c_CXX) $($c_CPPFLAGS) -MM -MP -MF $(@:%.o=%.d) -MT "$@" -MT "$(@:%.o=%.d)" $<

-include $($c_OBJECT_PATHS:%.o=%.d)

