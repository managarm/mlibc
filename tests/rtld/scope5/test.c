#include <dlfcn.h>
#include <stdio.h>
#include <assert.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#else
#define LIBFOO "libfoo.so"
#endif

// We need to have a relocation against foo for DT_NEEDED.
void foo();
void bar() { foo(); }

int main() {
	// In this test, we have exec -> foo (where '->' means 'depends on').
	// This means that foo is in the global scope due to DT_NEEDED.
	// We then dlopen it again with RTLD_LOCAL, which should just return
	// the already-loaded object, but used to crash in the mlibc linker instead.

	void *foo = dlopen(LIBFOO, RTLD_LOCAL | RTLD_NOW);
	assert(foo);
	assert(dlsym(foo, "foo"));
	assert(dlsym(RTLD_DEFAULT, "foo"));

	dlclose(foo);

	return 0;
}
