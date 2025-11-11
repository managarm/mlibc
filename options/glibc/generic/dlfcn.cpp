
#include <bits/ensure.h>
#include <dlfcn.h>

#include <mlibc/debug.hpp>

struct __dlapi_symbol {
	const char *file;
	void *base;
	const char *symbol;
	void *address;
	const void *elf_symbol;
	void *link_map;
};

extern "C" const char *__dlapi_error();
extern "C" void *__dlapi_open(const char *, int, void *);
extern "C" void *__dlapi_resolve(void *, const char *, void *, const char *);
extern "C" int __dlapi_reverse(const void *, __dlapi_symbol *);
extern "C" int __dlapi_close(void *);
extern "C" int __dlapi_find_object(void *__address, dl_find_object *__result);

int dladdr1(const void *ptr, Dl_info *out, void **extra, int flags) {
	__dlapi_symbol info;
	if(__dlapi_reverse(ptr, &info)) {
		return 0;
	}

	out->dli_fname = info.file;
	out->dli_fbase = info.base;
	out->dli_sname = info.symbol;
	out->dli_saddr = info.address;

	switch(flags) {
	case RTLD_DL_SYMENT:
		*const_cast<const void **>(extra) = info.elf_symbol;
		break;
	case RTLD_DL_LINKMAP:
		*extra = info.link_map;
		break;
	default:
		break;
	}

	return 1;
}

int dlinfo(void *__restrict, int, void *__restrict) {
	__ensure(!"dlinfo() not implemented");
	__builtin_unreachable();
}

int _dl_find_object(void *address, struct dl_find_object *result) {
	return __dlapi_find_object(address, result);
}
