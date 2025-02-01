#include <assert.h>
#include <dlfcn.h>

#ifdef USE_HOST_LIBC
#define LIBBAR "libnative-bar.so"
#else
#define LIBBAR "libbar.so"
#endif

int foo();
int bar();

int main() {
	void *libbar = dlopen(LIBBAR, RTLD_LOCAL | RTLD_NOW);
	assert(libbar);

	assert(foo() == (int) 0xCAFEBABE);
	assert(bar() == (int) 0xCAFEBABE);

	return 0;
}
