
#include <bits/ensure.h>
#include <dlfcn.h>

#include <mlibc/debug.hpp>
#include <mlibc/dlapi.hpp>

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
