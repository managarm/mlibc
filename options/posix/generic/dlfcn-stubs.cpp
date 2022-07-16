
#include <bits/ensure.h>
#include <dlfcn.h>

#include <mlibc/debug.hpp>

struct __dlapi_symbol {
	const char *file;
	void *base;
	const char *symbol;
	void *address;
};

extern "C" const char *__dlapi_error();
extern "C" void *__dlapi_open(const char *, int, void *);
extern "C" void *__dlapi_resolve(void *, const char *, void *);
extern "C" int __dlapi_reverse(const void *, __dlapi_symbol *);
extern "C" int __dlapi_close(void *);

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
	return __dlapi_resolve(handle, string, ra);
}

[[gnu::noinline]]
void *dlvsym(void *__restrict handle, const char *__restrict string, const char *__restrict version) {
	mlibc::infoLogger() << "mlibc: dlvsym ignores version " << version << frg::endlog;
	auto ra = __builtin_extract_return_addr(__builtin_return_address(0));
	return __dlapi_resolve(handle, string, ra);
}

//gnu extensions
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

int dlinfo(void *, int, void *) {
	__ensure(!"dlinfo() not implemented");
	__builtin_unreachable();
}

