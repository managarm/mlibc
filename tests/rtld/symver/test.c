#include <assert.h>
#include <stdio.h>
#include <dlfcn.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#else
#define LIBFOO "libfoo.so"
#endif

int foo(void);

int main() {
	int ver = foo();
	fprintf(stderr, "called foo version %d\n", ver);
	assert(ver == 3); // version 3 is the default for libfoo.so

	void *libfoo_h = dlopen(LIBFOO, RTLD_GLOBAL | RTLD_NOW);
	assert(libfoo_h);

	int (*foo1)(void) = dlvsym(libfoo_h, "foo", "FOO_1");
	assert(foo1);

	int (*foo2)(void) = dlvsym(libfoo_h, "foo", "FOO_2");
	assert(foo2);

	int (*foo3)(void) = dlvsym(libfoo_h, "foo", "FOO_3");
	assert(foo3);

	int (*foo_def)(void) = dlsym(libfoo_h, "foo");
	assert(foo_def);

	assert(foo1() == 1);
	assert(foo2() == 2);
	assert(foo3() == 3);
	assert(foo3 == foo_def);

	return 0;
}
