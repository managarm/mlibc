
$c_SRCDIR = $(TREE_PATH)/$c/src
$c_HEADERDIR := $(TREE_PATH)/$c/include
$c_OBJDIR := $(BUILD_PATH)/$c/obj

$c_HEADERS := stdint.h \
	mlibc/null.h mlibc/size_t.h mlibc/wchar_t.h

$c_OBJECTS := initfini.o
$c_OBJECT_PATHS := $(addprefix $($c_OBJDIR)/,$($c_OBJECTS))

$c_EXTRA_OBJECTS := mlibc_begin.o mlibc_end.o
$c_EXTRA_OBJECT_PATHS := $(addprefix $($c_OBJDIR)/,$($c_EXTRA_OBJECTS))

$c_AS := x86_64-managarm-as

$c_CXX := x86_64-managarm-g++
$c_CPPFLAGS := -std=c++14 -Wall
$c_CXXFLAGS := $($c_CPPFLAGS) -fPIC -O2
$c_CXXFLAGS += -fno-builtin -fno-rtti -fno-exceptions

$c_TARGETS := clean-$c install-headers-$c $($c_OBJECT_PATHS) $($c_EXTRA_OBJECT_PATHS)

.PHONY: clean-$c install-headers-$c

clean-$c:
	rm -f $($c_OBJECT_PATHS) $($c_EXTRA_OBJECT_PATHS)
	rm -f $($c_OBJECT_PATHS:%.o=%.d) $($c_EXTRA_OBJECT_PATHS:%.o=%.d)

install-headers-$c:
	mkdir -p  $(SYSROOT_PATH)/usr/include/mlibc
	for f in $($c_HEADERS); do \
		install -p $($c_HEADERDIR)/$$f $(SYSROOT_PATH)/usr/include/$$f; done

$($c_OBJDIR):
	mkdir -p $@

$($c_OBJDIR)/%.o: $($c_SRCDIR)/%.S | $($c_OBJDIR)
	$($c_AS) -o $@ $($c_ASFLAGS) $<

$($c_OBJDIR)/%.o: $($c_SRCDIR)/%.cpp | $($c_OBJDIR)
	$($c_CXX) -c -o $@ $($c_CXXFLAGS) $<
	$($c_CXX) $($c_CPPFLAGS) -MM -MP -MF $(@:%.o=%.d) -MT "$@" -MT "$(@:%.o=%.d)" $<

-include $($c_OBJECT_PATHS:%.o=%.d)
