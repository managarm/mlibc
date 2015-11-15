
$c_SRCDIR = $(TREE_PATH)/$c/src
$c_HEADERDIR := $(TREE_PATH)/$c/include
$c_OBJDIR := $(BUILD_PATH)/$c/obj

$c_HEADERS := assert.h ctype.h errno.h signal.h stdio.h stdlib.h string.h time.h \
	mlibc/ensure.h

$c_OBJECTS := assert.o
$c_OBJECT_PATHS := $(addprefix $($c_OBJDIR)/,$($c_OBJECTS))

$c_CXX := g++
$c_CPPFLAGS := -std=c++11 -nostdinc -Wall
$c_CPPFLAGS := -I$(TREE_PATH)/libc/generic/ansi/include
$c_CXXFLAGS :=  $($c_CPPFLAGS) -fPIC -O2

$c_TARGETS := clean-$c install-$c $($c_OBJECT_PATHS)

.PHONY: clean-$c install-$c

clean-$c:
	rm -f $($c_OBJECT_PATHS)
	rm -f $($c_OBJECT_PATHS:%.o=%.d)

install-$c:
	mkdir -p  $(SYSROOT_PATH)/usr/include
	mkdir -p  $(SYSROOT_PATH)/usr/include/mlibc
	for f in $($c_HEADERS); do \
		install $($c_HEADERDIR)/$$f $(SYSROOT_PATH)/usr/include/$$f; done

$($c_OBJDIR):
	mkdir -p $@

$($c_OBJDIR)/%.o: $($c_SRCDIR)/%.cpp | $($c_OBJDIR)
	$($c_CXX) -c -o $@ $($c_CXXFLAGS) $<
	$($c_CXX) $($c_CPPFLAGS) -MM -MP -MF $(@:%.o=%.d) -MT "$@" -MT "$(@:%.o=%.d)" $<

-include $($c_OBJECT_PATHS:%.o=%.d)

