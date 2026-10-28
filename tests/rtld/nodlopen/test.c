#include <assert.h>
#include <dlfcn.h>
#include <stddef.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#else
#define LIBFOO "libfoo.so"
#endif

int main() {
	// libfoo is linked with -z nodlopen, which sets DF_1_NOOPEN.
	assert(dlopen(LIBFOO, RTLD_NOW) == NULL);
	assert(dlerror() != NULL);
	return 0;
}
