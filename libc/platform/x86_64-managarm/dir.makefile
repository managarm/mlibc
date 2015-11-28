
$c_SRCDIR = $(TREE_PATH)/$c/src
$c_BINDIR := $(BUILD_PATH)/$c/bin

$c_CXX := x86_64-managarm-g++
$c_CPPFLAGS := -std=c++11 -Wall
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/ansi/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/compilers/gcc/include
$c_CXXFLAGS :=  $($c_CPPFLAGS) -fPIC -O2

$c_TARGETS := all-$c clean-$c install-$c $($c_BINDIR)/crt0.o

.PHONY: all-$c clean-$c install-$c

all-$c: $($c_BINDIR)/crt0.o

clean-$c:
	rm -f $($c_BINDIR)/crt0.o
	rm -f $($c_BINDIR)/crt0.d

install-$c:
	mkdir -p  $(SYSROOT_PATH)/usr/lib
	install $($c_BINDIR)/crt0.o $(SYSROOT_PATH)/usr/lib

$($c_BINDIR):
	mkdir -p $@

# compile crt0.o
$($c_BINDIR)/crt0.o: $($c_SRCDIR)/crt0.cpp | $($c_BINDIR)
	$($c_CXX) -c -o $@ $($c_CXXFLAGS) $<
	$($c_CXX) $($c_CPPFLAGS) -MM -MP -MF $(@:%.o=%.d) -MT "$@" -MT "$(@:%.o=%.d)" $<

-include $($c_BINDIR)/crt0.d

