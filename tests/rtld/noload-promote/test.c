#include <dlfcn.h>
#include <assert.h>
#include <stddef.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#else
#define LIBFOO "libfoo.so"
#endif

int main() {
	void *foo = dlopen(LIBFOO, RTLD_LOCAL | RTLD_NOW);
	assert(foo);

	assert(dlsym(RTLD_DEFAULT, "foo") == NULL);

	// Opening a library with RTLD_NOLOAD | RTLD_GLOBAL should promote it to the global scope.
	assert(dlopen(LIBFOO, RTLD_NOLOAD | RTLD_GLOBAL | RTLD_NOW) == foo);
	assert(dlsym(RTLD_DEFAULT, "foo") != NULL);

	assert(dlopen("does-not-exist.so.1337", RTLD_NOLOAD | RTLD_GLOBAL | RTLD_NOW) == NULL);

	return 0;
}
