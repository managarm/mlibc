
$c_HEADERDIR := $(TREE_PATH)/$c/include

$c_HEADERS := stdint.h \
	mlibc/null.h mlibc/size_t.h mlibc/wchar_t.h

$c_TARGETS := install-headers-$c

.PHONY: install-headers-$c

install-headers-$c:
	mkdir -p  $(SYSROOT_PATH)/usr/include/mlibc
	for f in $($c_HEADERS); do \
		install -p $($c_HEADERDIR)/$$f $(SYSROOT_PATH)/usr/include/$$f; done

