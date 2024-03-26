#include <dlfcn.h>
#include <stdio.h>
#include <assert.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#define LIBBAR "libnative-bar.so"
#define LIBBAZ "libnative-baz.so"
#else
#define LIBFOO "libfoo.so"
#define LIBBAR "libbar.so"
#define LIBBAZ "libbaz.so"
#endif

int main() {
	// In this test, we have foo -> baz, bar -> foo (where '->' means 'depends on').
	// We first load foo with RTLD_LOCAL, and then load bar with RTLD_GLOBAL.
	// This should bring foo and bar into the global scope.

	void *foo = dlopen(LIBFOO, RTLD_LOCAL | RTLD_NOW);
	assert(foo);
	assert(dlsym(foo, "foo"));
	assert(dlsym(foo, "baz"));
	assert(!dlsym(RTLD_DEFAULT, "foo"));
	assert(!dlsym(RTLD_DEFAULT, "baz"));

	void *bar = dlopen(LIBBAR, RTLD_GLOBAL | RTLD_NOW);
	assert(bar);
	assert(dlsym(bar, "bar"));
	assert(dlsym(RTLD_DEFAULT, "bar"));
	assert(dlsym(RTLD_DEFAULT, "foo"));
	assert(dlsym(RTLD_DEFAULT, "baz"));

	dlclose(foo);
	dlclose(bar);

	return 0;
}
