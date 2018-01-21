
libc_CXX := x86_64-managarm-g++

libc_CPPFLAGS := -std=c++1z -Wall
libc_CPPFLAGS += -I$(TREE_PATH)/libc/compilers/gcc/private
libc_CPPFLAGS += -I$(TREE_PATH)/libc/generic/ansi/include
libc_CPPFLAGS += -I$(TREE_PATH)/libc/generic/linux/include
libc_CPPFLAGS += -I$(TREE_PATH)/libc/generic/lsb/include
libc_CPPFLAGS += -I$(TREE_PATH)/libc/generic/posix/include
libc_CPPFLAGS += -I$(TREE_PATH)/libc/compilers/gcc/include
libc_CPPFLAGS += -I$(TREE_PATH)/libc/machine/x86_64/include
libc_CPPFLAGS += -I$(TREE_PATH)/libc/platform/x86_64-managarm/include
libc_CPPFLAGS += -Igen
libc_CPPFLAGS += -I$(FRIGG_PATH)/include
libc_CPPFLAGS += -DFRIGG_HAVE_LIBC -DFRIGG_HIDDEN

libc_CXXFLAGS := $(libc_CPPFLAGS) -fPIC -O2
libc_CXXFLAGS += -fno-builtin -fno-rtti -fno-exceptions

libc_BEGIN := libc/compilers/gcc/extra-src/mlibc_crtbegin.o
libc_END := libc/compilers/gcc/extra-src/mlibc_crtend.o

libc_gendir := gen/

libc_dirs := libc/generic/ansi/src libc/generic/linux/src
libc_dirs += libc/generic/lsb/src libc/generic/posix/src
libc_dirs += libc/compilers/gcc/src libc/compilers/gcc/extra-src
libc_dirs += libc/machine/x86_64/src
libc_dirs += libc/platform/x86_64-managarm/src

libc_cxx_sources := $(wildcard $(TREE_PATH)/libc/generic/ansi/src/*.cpp)
libc_cxx_sources += $(wildcard $(TREE_PATH)/libc/generic/linux/src/*.cpp)
libc_cxx_sources += $(wildcard $(TREE_PATH)/libc/generic/lsb/src/*.cpp)
libc_cxx_sources += $(wildcard $(TREE_PATH)/libc/generic/posix/src/*.cpp)
libc_cxx_sources += $(wildcard $(TREE_PATH)/libc/compilers/gcc/src/*.cpp)
libc_cxx_sources += $(wildcard $(TREE_PATH)/libc/platform/x86_64-managarm/src/*.cpp)

libc_s_sources := $(wildcard $(TREE_PATH)/libc/machine/x86_64/src/*.S)

libc_objects := $(patsubst $(TREE_PATH)/%.cpp,%.o,$(libc_cxx_sources))
libc_objects += $(patsubst $(TREE_PATH)/%.cpp,%.o,$(libc_s_sources))

libc_AS := x86_64-managarm-as
libc_ASFLAGS := 

vpath %.S $(TREE_PATH)
vpath %.cpp $(TREE_PATH)

$(libc_dirs):
	mkdir -p $@

$(libc_gendir):
	mkdir -p $@

%.o: %.S | $(libc_dirs)
	$(libc_AS) -o $@ $($libc_ASFLAGS) $<

%.o: %.cpp | $(libc_dirs)
	$(libc_CXX) -c -o $@ $(libc_CXXFLAGS) $<
	$(libc_CXX) $(libc_CPPFLAGS) -MM -MP -MF $(@:%.o=%.d) -MT "$@" -MT "$(@:%.o=%.d)" $<

libc.so: $(libc_objects) $(libc_BEGIN) $(libc_END)
	x86_64-managarm-g++ -shared -o $@ -nostdlib $(libc_BEGIN) $(libc_objects) -l:ld-init.so $(libc_END)

gen-libc: $(libc_gendir)/posix.frigg_pb.hpp $(libc_gendir)/fs.frigg_pb.hpp

install-libc:
	mkdir -p $(SYSROOT_PATH)/usr/lib
	install -p libc.so $(SYSROOT_PATH)/usr/lib

$(libc_gendir)/%.frigg_pb.hpp: $(MANAGARM_SRC_PATH)/bragi/proto/%.proto | $(libc_gendir)
	$(PROTOC) --plugin=protoc-gen-frigg=$(MANAGARM_BUILD_PATH)/tools/frigg_pb/bin/frigg_pb \
			--frigg_out=$(libc_gendir) --proto_path=$(MANAGARM_SRC_PATH)/bragi/proto $<
	
