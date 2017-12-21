
#include <dlfcn.h>

#include <mlibc/ensure.h>

extern "C" void *__dlapi_open(const char *);
extern "C" void *__dlapi_resolve(void *handle, const char *);

int dlclose(void *) {
	__ensure(!"dlclose() not implemented");
	__builtin_unreachable();
}

char *dlerror(void) {
	__ensure(!"dlerror() not implemented");
	__builtin_unreachable();
}

void *dlopen(const char *file, int flags) {
	__ensure(flags & RTLD_GLOBAL);
	return __dlapi_open(file);
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

