
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
extern "C" void *__dlapi_open(const char *, int);
extern "C" void *__dlapi_resolve(void *, const char *);
extern "C" int __dlapi_reverse(const void *, __dlapi_symbol *);

int dlclose(void *) {
	mlibc::infoLogger() << "\e[31mmlibc: dlclose() is a no-op\e[39m" << frg::endlog;
	return 0;
}

char *dlerror(void) {
	return const_cast<char *>(__dlapi_error());
}

void *dlopen(const char *file, int flags) {
	// The following three flags are glibc extensions.
	// TODO: Validate the flags.
	if(flags & RTLD_NOLOAD) {
		mlibc::infoLogger() << "\e[31mmlibc: dlopen(RTLD_NOLOAD) always fails\e[39m"
				<< frg::endlog;
		return nullptr;
	}

	int unhandled = flags & (RTLD_NODELETE | RTLD_DEEPBIND);
	if (unhandled)
		mlibc::infoLogger() << "mlibc: unhandled dlopen flags " << unhandled << frg::endlog;

	return __dlapi_open(file, !(flags & RTLD_GLOBAL));
}

void *dlsym(void *__restrict handle, const char *__restrict string) {
	return __dlapi_resolve(handle, string);
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

