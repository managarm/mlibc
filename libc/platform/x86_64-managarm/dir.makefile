
$c_SRCDIR = $(TREE_PATH)/$c/src
$c_CRT_SRCDIR = $(TREE_PATH)/$c/crt-src
$c_GENDIR := $(BUILD_PATH)/$c/gen
$c_OBJDIR := $(BUILD_PATH)/$c/obj
$c_BINDIR := $(BUILD_PATH)/$c/bin

$c_OBJECTS := entry.o ensure.o malloc.o file.o \
	fork-exec.o enter-fork.o signals.o host.o time.o \
	frigg-support.o
$c_OBJECT_PATHS := $(addprefix $($c_OBJDIR)/,$($c_OBJECTS))

$c_AS := x86_64-managarm-as 

$c_CXX := x86_64-managarm-g++
$c_CPPFLAGS := -std=c++11 -Wall
$c_CPPFLAGS += -I$(FRIGG_PATH)/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/ansi/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/posix/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/compilers/gcc/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/platform/x86_64-managarm/include
$c_CPPFLAGS += -I$($c_GENDIR)
$c_CPPFLAGS += -DFRIGG_HAVE_LIBC
$c_CXXFLAGS :=  $($c_CPPFLAGS) -fPIC -O2
$c_CXXFLAGS += -fno-builtin -fno-rtti -fno-exceptions

$c_TARGETS := all-$c clean-$c install-$c gen-$c $($c_BINDIR)/crt0.o $($c_OBJECT_PATHS)

.PHONY: all-$c clean-$c install-$c

all-$c: $($c_BINDIR)/crt0.o

clean-$c:
	rm -f $($c_BINDIR)/crt0.o
	rm -f $($c_BINDIR)/crt0.d

install-$c:
	mkdir -p  $(SYSROOT_PATH)/usr/lib
	install -p $($c_BINDIR)/crt0.o $(SYSROOT_PATH)/usr/lib

gen-$c: $($c_GENDIR)/posix.frigg_pb.hpp

$($c_GENDIR) $($c_OBJDIR) $($c_BINDIR):
	mkdir -p $@

# compile library object files
$($c_OBJDIR)/%.o: $($c_SRCDIR)/%.S | $($c_OBJDIR)
	$($c_AS) -c -o $@ $($c_ASFLAGS) $<

$($c_OBJDIR)/%.o: $($c_SRCDIR)/%.cpp | $($c_OBJDIR)
	$($c_CXX) -c -o $@ $($c_CXXFLAGS) $<
	$($c_CXX) $($c_CPPFLAGS) -MM -MP -MF $(@:%.o=%.d) -MT "$@" -MT "$(@:%.o=%.d)" $<

-include $($c_OBJECT_PATHS:%.o=%.d)

# compile crt0.o
$($c_BINDIR)/crt0.o: $($c_CRT_SRCDIR)/crt0.S | $($c_BINDIR)
	$($c_AS) -c -o $@ $($c_ASFLAGS) $<

# generate protobuf files
$c_TARGETS += $($c_GENDIR)/%
$($c_GENDIR)/%.frigg_pb.hpp: $(MANAGARM_SRC_PATH)/bragi/proto/%.proto | $($c_GENDIR)
	$(PROTOC) --plugin=protoc-gen-frigg=$(MANAGARM_BUILD_PATH)/tools/frigg_pb/bin/frigg_pb \
			--frigg_out=$($c_GENDIR) --proto_path=$(MANAGARM_SRC_PATH)/bragi/proto $<

-include $($c_BINDIR)/crt0.d

