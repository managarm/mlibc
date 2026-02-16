
#include <bits/ensure.h>
#include <dlfcn.h>

#include <mlibc/debug.hpp>
#include <mlibc/dlapi.hpp>

int dlclose(void *handle) {
	return __dlapi_close(handle);
}

char *dlerror(void) {
	return const_cast<char *>(__dlapi_error());
}

[[gnu::noinline]]
void *dlopen(const char *file, int flags) {
	auto ra = __builtin_extract_return_addr(__builtin_return_address(0));
	return __dlapi_open(file, flags, ra);
}

[[gnu::noinline]]
void *dlsym(void *__restrict handle, const char *__restrict string) {
	auto ra = __builtin_extract_return_addr(__builtin_return_address(0));
	return __dlapi_resolve(handle, string, ra, nullptr);
}

[[gnu::noinline]]
void *dlvsym(void *__restrict handle, const char *__restrict string, const char *__restrict version) {
	auto ra = __builtin_extract_return_addr(__builtin_return_address(0));
	return __dlapi_resolve(handle, string, ra, version);
}

int dladdr(const void *ptr, Dl_info *out) {
	__dlapi_symbol info;
	if(__dlapi_reverse(ptr, &info))
		return 0;

	out->dli_fname = info.file;
	out->dli_fbase = info.base;
	out->dli_sname = info.symbol;
	out->dli_saddr = info.address;
	return 1;
}
