#include <link.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

extern "C" int __dlapi_iterate_phdr(int (*)(struct dl_phdr_info*, size_t, void*), void *);

int dl_iterate_phdr(int (*callback)(struct dl_phdr_info*, size_t, void*), void *data) {
	return __dlapi_iterate_phdr(callback, data);
}
