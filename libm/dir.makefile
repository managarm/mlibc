
$c_SRCDIR := $(TREE_PATH)/$c/src
$c_BINDIR := $(BUILD_PATH)/$c/bin
$c_OBJDIR := $(BUILD_PATH)/$c/obj

$c_LIBRARY_OBJS := $($c_OBJDIR)/dummy.o

$c_LDFLAGS := -nodefaultlibs

$c_TARGETS := clean-$c install-$c $($c_BINDIR)/libm.so

.PHONY: all-$c install-$c

all-$c: $($c_BINDIR)/libm.so

clean-$c:
	rm -f $($c_BINDIR)/libm.so

$($c_OBJDIR) $($c_BINDIR):
	mkdir -p $@

$($c_OBJDIR)/%.o: $($c_SRCDIR)/%.cpp | $($c_OBJDIR)
	x86_64-managarm-g++ -c -o $@ $<

$($c_BINDIR)/libm.so: $($c_LIBRARY_OBJS) | $($c_BINDIR)
	x86_64-managarm-g++ -shared -o $@ $($c_LDFLAGS) $($c_LIBRARY_OBJS)

install-$c:
	mkdir -p $(SYSROOT_PATH)/usr/lib
	install $($c_BINDIR)/libm.so $(SYSROOT_PATH)/usr/lib

