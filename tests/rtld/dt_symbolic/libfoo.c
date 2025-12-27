#include <assert.h>
#include <dlfcn.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#else
#define LIBFOO "libfoo.so"
#endif

int foo(void) {
	return 1;
}

int bar(void) {
	typeof(int (void)) *x = (typeof(int (void)) *) dlsym(RTLD_DEFAULT, "foo");
	void *h = dlopen(LIBFOO, RTLD_NOW);
	typeof(int (void)) *y = (typeof(int (void)) *) dlsym(h, "foo");
	assert(x == y);
	return x();
}
