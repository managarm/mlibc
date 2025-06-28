#include <assert.h>
#include <dlfcn.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#else
#define LIBFOO "libfoo.so"
#endif

int foo(void);

int bar(void) {
	return foo();
}

[[gnu::constructor]] void init(void) {
	void *libfoo = dlopen(LIBFOO, RTLD_LOCAL | RTLD_NOW);
	assert(libfoo);
}
