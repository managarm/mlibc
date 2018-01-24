
libc_CXX := x86_64-managarm-g++

libc_CPPFLAGS := -std=c++1z -Wall
libc_CPPFLAGS += -I$(TREE_PATH)/options/internal/private
libc_CPPFLAGS += -I$(TREE_PATH)/options/ansi/include
libc_CPPFLAGS += -I$(TREE_PATH)/options/linux/include
libc_CPPFLAGS += -I$(TREE_PATH)/options/lsb/include
libc_CPPFLAGS += -I$(TREE_PATH)/options/posix/include
libc_CPPFLAGS += -I$(TREE_PATH)/options/internal/include
libc_CPPFLAGS += -I$(TREE_PATH)/sysdeps/managarm/include
libc_CPPFLAGS += -Igen
libc_CPPFLAGS += -I$(FRIGG_PATH)/include
libc_CPPFLAGS += -DFRIGG_HAVE_LIBC -DFRIGG_HIDDEN

libc_CXXFLAGS := $(libc_CPPFLAGS) -fPIC -O2
libc_CXXFLAGS += -fno-builtin -fno-rtti -fno-exceptions

libc_BEGIN := options/internal/gcc-extra/mlibc_crtbegin.o
libc_END := options/internal/gcc-extra/mlibc_crtend.o

libc_gendir := gen/

libc_includes := sys/socket.h

$(SYSROOT_PATH)/usr/include/%.h: options/posix/include/%.h
	install -Dp $< $@

$(SYSROOT_PATH)/usr/lib/libc.so: libc.so
	install -Dp $< $@

libc_code_dirs := options/ansi/generic
libc_code_dirs += options/linux/generic
libc_code_dirs += options/lsb/generic
libc_code_dirs += options/posix/generic
libc_code_dirs += options/internal/gcc \
	options/internal/gcc-extra \
	options/internal/x86_64
libc_code_dirs += sysdeps/managarm/generic

libc_s_sources := $(wildcard $(TREE_PATH)/options/internal/x86_64/*.S)

libc_cxx_sources := $(wildcard $(TREE_PATH)/options/ansi/generic/*.cpp)
libc_cxx_sources += $(wildcard $(TREE_PATH)/options/linux/generic/*.cpp)
libc_cxx_sources += $(wildcard $(TREE_PATH)/options/lsb/generic/*.cpp)
libc_cxx_sources += $(wildcard $(TREE_PATH)/options/posix/generic/*.cpp)
libc_cxx_sources += $(wildcard $(TREE_PATH)/options/internal/gcc/*.cpp)
libc_cxx_sources += $(wildcard $(TREE_PATH)/sysdeps/managarm/generic/*.cpp)

libc_objects := $(patsubst $(TREE_PATH)/%.cpp,%.o,$(libc_s_sources))
libc_objects += $(patsubst $(TREE_PATH)/%.cpp,%.o,$(libc_cxx_sources))

libc_AS := x86_64-managarm-as
libc_ASFLAGS := 

vpath %.h $(TREE_PATH)
vpath %.S $(TREE_PATH)
vpath %.cpp $(TREE_PATH)

$(libc_code_dirs):
	mkdir -p $@

$(libc_gendir):
	mkdir -p $@

%.o: %.S | $(libc_code_dirs)
	$(libc_AS) -o $@ $($libc_ASFLAGS) $<

%.o: %.cpp | $(libc_code_dirs)
	$(libc_CXX) -c -o $@ $(libc_CXXFLAGS) $<
	$(libc_CXX) $(libc_CPPFLAGS) -MM -MP -MF $(@:%.o=%.d) -MT "$@" -MT "$(@:%.o=%.d)" $<

libc.so: $(libc_objects) $(libc_BEGIN) $(libc_END)
	x86_64-managarm-g++ -shared -o $@ -nostdlib $(libc_BEGIN) $(libc_objects) -l:ld-init.so $(libc_END)

gen-libc: $(libc_gendir)/posix.frigg_pb.hpp $(libc_gendir)/fs.frigg_pb.hpp

install-headers-libc: $(addprefix $(SYSROOT_PATH)/usr/include/,$(libc_includes))

install-libc: $(addprefix $(SYSROOT_PATH)/usr/include/,$(libc_includes))
install-libc: $(SYSROOT_PATH)/usr/lib/libc.so

$(libc_gendir)/%.frigg_pb.hpp: $(MANAGARM_SRC_PATH)/bragi/proto/%.proto | $(libc_gendir)
	$(PROTOC) --plugin=protoc-gen-frigg=$(MANAGARM_BUILD_PATH)/tools/frigg_pb/bin/frigg_pb \
			--frigg_out=$(libc_gendir) --proto_path=$(MANAGARM_SRC_PATH)/bragi/proto $<

