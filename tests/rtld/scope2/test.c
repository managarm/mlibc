#include <stddef.h>
#include <dlfcn.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#ifdef USE_HOST_LIBC
#define LIBBAR "libnative-bar.so"
#define LIBBAZ "libnative-baz.so"
#else
#define LIBBAR "libbar.so"
#define LIBBAZ "libbaz.so"
#endif

typedef char *strfn(void);

int main() {
	void *baz = dlopen(LIBBAZ, RTLD_LAZY | RTLD_GLOBAL);
	assert(baz);

	// At this point, baz is loaded in the global scope. When we load bar locally,
	// there is a relocation to `foo_baz_conflict` which is defined in both
	// foo (which is a dependency of bar), and baz. In this case baz should win
	// since we search the global scope first.

	void *bar = dlopen(LIBBAR, RTLD_LAZY | RTLD_LOCAL);
	assert(bar);

	strfn *bfn = dlsym(bar, "bar_calls_foo_baz_conflict");
	assert(!strcmp(bfn(), "resolved to baz"));

	// TODO: Test RTLD_DEEPBIND and DT_SYMBOLIC once we implement it.

	dlclose(bar);
	dlclose(baz);

	return 0;
}
