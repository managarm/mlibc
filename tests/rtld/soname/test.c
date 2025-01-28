#include <dlfcn.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#define LIBBAR "libnative-bar.so"
#else
#define LIBFOO "libfoo.so"
#define LIBBAR "libbar.so"
#endif

int main() {
	void *foo = dlopen(LIBFOO, RTLD_NOW);
	void *bar = dlopen(LIBBAR, RTLD_NOW);
	assert(foo);
	assert(bar);

	// Since these libraries have the same SONAME, they should return the same thing.
	assert(foo == bar);

	char *(*fooSym)(void) = dlsym(foo, "name");
	char *(*barSym)(void) = dlsym(bar, "name");
	assert(fooSym && barSym);
	assert(fooSym() && barSym());
	printf("foo: name() = \"%s\"\n", fooSym());
	printf("bar: name() = \"%s\"\n", barSym());
	assert(!strcmp(fooSym(), barSym()));

	dlclose(foo);
	dlclose(bar);

	return 0;
}
