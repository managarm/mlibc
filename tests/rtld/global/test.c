#include <assert.h>
#include <dlfcn.h>
#include <stddef.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#define LIBBAR "libnative-bar.so"
#else
#define LIBFOO "libfoo.so"
#define LIBBAR "libbar.so"
#endif

int main() {
	// libfoo is linked with -z global, which sets DF_1_GLOBAL,
	// so it should behave as if it was opened with RTLD_GLOBAL.
	void *foo = dlopen(LIBFOO, RTLD_LOCAL | RTLD_NOW);
	assert(foo);
	assert(dlsym(RTLD_DEFAULT, "foo") != NULL);

	// libbar is not linked against libfoo, but can still use its symbols.
	void *bar = dlopen(LIBBAR, RTLD_LOCAL | RTLD_NOW);
	assert(bar);

	int (*bar_fn)(void) = (int (*)(void))dlsym(bar, "bar");
	assert(bar_fn);
	assert(bar_fn() == 42);
	return 0;
}
