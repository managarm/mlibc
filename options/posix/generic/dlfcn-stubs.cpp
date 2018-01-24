
#include <dlfcn.h>

#include <bits/ensure.h>

extern "C" const char *__dlapi_error();
extern "C" void *__dlapi_open(const char *, int);
extern "C" void *__dlapi_resolve(void *, const char *);

int dlclose(void *) {
	__ensure(!"dlclose() not implemented");
	__builtin_unreachable();
}

char *dlerror(void) {
	return const_cast<char *>(__dlapi_error());
}

void *dlopen(const char *file, int flags) {
	return __dlapi_open(file, !(flags & RTLD_GLOBAL));
}

void *dlsym(void *__restrict handle, const char *__restrict string) {
	return __dlapi_resolve(handle, string);
}

//gnu extensions
int dladdr(const void *, Dl_info *) {
	__ensure(!"dladdr() not implemented");
	__builtin_unreachable();
}

int dlinfo(void *, int, void *) {
	__ensure(!"dlinfo() not implemented");
	__builtin_unreachable();
}

