
$c_OBJDIR := $(BUILD_PATH)/$c/obj

$c_OBJECTS := debug.o
$c_OBJECT_PATHS := $(addprefix $($c_OBJDIR)/,$($c_OBJECTS))

$c_CXX := x86_64-managarm-g++
$c_CPPFLAGS := -std=c++11 -Wall
$c_CPPFLAGS += -I$(FRIGG_PATH)/include
$c_CPPFLAGS += -DFRIGG_HAVE_LIBC -DFRIGG_HIDDEN
$c_CXXFLAGS :=  $($c_CPPFLAGS) -fPIC -O2

$c_TARGETS := clean-$c install-$c $($c_OBJECT_PATHS)

.PHONY: clean-$c install-$c

clean-$c:
	rm -f $($c_OBJECT_PATHS)
	rm -f $($c_OBJECT_PATHS:%.o=%.d)

$($c_OBJDIR):
	mkdir -p $@

$($c_OBJDIR)/%.o: $(FRIGG_PATH)/src/%.cpp | $($c_OBJDIR)
	$($c_CXX) -c -o $@ $($c_CXXFLAGS) $<
	$($c_CXX) $($c_CPPFLAGS) -MM -MP -MF $(@:%.o=%.d) -MT "$@" -MT "$(@:%.o=%.d)" $<

-include $($c_OBJECT_PATHS:%.o=%.d)


