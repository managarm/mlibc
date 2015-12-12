
$c_SRCDIR = $(TREE_PATH)/$c/src
$c_HEADERDIR := $(TREE_PATH)/$c/include
$c_OBJDIR := $(BUILD_PATH)/$c/obj

$c_HEADERS := dirent.h fcntl.h pthread.h pwd.h sched.h spawn.h strings.h termios.h unistd.h \
	sys/types.h sys/select.h sys/stat.h sys/time.h sys/wait.h \
	mlibc/ssize_t.h mlibc/uid_t.h mlibc/gid_t.h mlibc/pid_t.h mlibc/off_t.h \
	mlibc/mode_t.h mlibc/dev_t.h mlibc/ino_t.h mlibc/blksize_t.h mlibc/blkcnt_t.h \
	mlibc/nlink_t.h mlibc/suseconds_t.h mlibc/timeval.h \
	mlibc/posix_errno.h mlibc/posix_signal.h mlibc/posix_stdio.h mlibc/posix_string.h \
	mlibc/file.h

$c_OBJECTS := dirent-stubs.o fcntl-stubs.o pthread-stubs.o pwd-stubs.o sched-stubs.o \
	spawn-stubs.o strings-stubs.o termios-stubs.o unistd-stubs.o \
	sys-stat-stubs.o sys-select-stubs.o sys-time-stubs.o sys-wait-stubs.o \
	posix_stdio.o posix_string.o \
	file-io.o environment.o
$c_OBJECT_PATHS := $(addprefix $($c_OBJDIR)/,$($c_OBJECTS))

$c_CXX := x86_64-managarm-g++
$c_CPPFLAGS := -std=c++11 -Wall
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/ansi/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/posix/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/compilers/gcc/include
$c_CXXFLAGS := $($c_CPPFLAGS) -fPIC -O2
$c_CXXFLAGS += -fno-builtin -fno-rtti -fno-exceptions

$c_TARGETS := clean-$c install-$c $($c_OBJECT_PATHS)

.PHONY: clean-$c install-$c

clean-$c:
	rm -f $($c_OBJECT_PATHS)
	rm -f $($c_OBJECT_PATHS:%.o=%.d)

install-$c:
	mkdir -p  $(SYSROOT_PATH)/usr/include
	mkdir -p  $(SYSROOT_PATH)/usr/include/sys
	mkdir -p  $(SYSROOT_PATH)/usr/include/mlibc
	for f in $($c_HEADERS); do \
		install -p $($c_HEADERDIR)/$$f $(SYSROOT_PATH)/usr/include/$$f; done

$($c_OBJDIR):
	mkdir -p $@

$($c_OBJDIR)/%.o: $($c_SRCDIR)/%.cpp | $($c_OBJDIR)
	$($c_CXX) -c -o $@ $($c_CXXFLAGS) $<
	$($c_CXX) $($c_CPPFLAGS) -MM -MP -MF $(@:%.o=%.d) -MT "$@" -MT "$(@:%.o=%.d)" $<

-include $($c_OBJECT_PATHS:%.o=%.d)

