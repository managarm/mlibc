#include <dlfcn.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#else
#define LIBFOO "libfoo.so"
#endif

int main() {
	void *foo = dlopen(LIBFOO, RTLD_NOW);
	assert(foo);
	dlclose(foo);

	return 0;
}
