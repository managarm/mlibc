
$c_BINDIR := $(BUILD_PATH)/$c/bin

$c_LIBRARY_OBJS :=

$c_LDFLAGS := -nostdlib

$c_TARGETS := clean-$c $($c_BINDIR)/libc.so

.PHONY: all-$c install-$c

all-$c: $($c_BINDIR)/libc.so

clean-$c:
	rm -f $($c_BINDIR)/libc.so

$(call include_dir,$c/generic/ansi)
$c_LIBRARY_OBJS += $($c/generic/ansi_OBJECT_PATHS)
clean-$c: clean-$c/generic/ansi
install-$c: install-$c/generic/ansi

$($c_BINDIR):
	mkdir -p $@

$($c_BINDIR)/libc.so: $($c_LIBRARY_OBJS) | $($c_BINDIR)
	g++ -shared -o $@ $($c_LDFLAGS) $($c_LIBRARY_OBJS)

