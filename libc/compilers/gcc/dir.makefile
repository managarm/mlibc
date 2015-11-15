
$c_HEADERDIR := $(TREE_PATH)/$c/include

$c_HEADERS := mlibc/null.h mlibc/size_t.h mlibc/wchar_t.h

$c_TARGETS := install-$c

.PHONY: install-$c

install-$c:
	mkdir -p  $(SYSROOT_PATH)/usr/include/mlibc
	for f in $($c_HEADERS); do \
		install $($c_HEADERDIR)/$$f $(SYSROOT_PATH)/usr/include/$$f; done

