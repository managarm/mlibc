
#include <dlfcn.h>

#include <mlibc/ensure.h>

int dlclose(void *) {
	__ensure(!"dlclose() not implemented");
	__builtin_unreachable();
}

char *dlerror(void) {
	__ensure(!"dlerror() not implemented");
	__builtin_unreachable();
}

void *dlopen(const char *, int) {
	__ensure(!"dlopen() not implemented");
	__builtin_unreachable();
}

void *dlsym(void *__restrict, const char *__restrict) {
	__ensure(!"dlsym() not implemented");
	__builtin_unreachable();
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

