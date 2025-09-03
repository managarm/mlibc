#include <assert.h>
#include <dlfcn.h>

int foo(void);

#ifdef USE_HOST_LIBC
#define LIB "libnative-ifunc-test-dlopen.so"
#else
#define LIB "libifunc-test-dlopen.so"
#endif

int main(void) {
	void *handle = dlopen(LIB, RTLD_NOW | RTLD_LOCAL);
	assert(handle);

	int (*func)() = (int (*)()) dlsym(handle, "foo");
	assert(func);

	int ret = func();
	assert(ret == 69);

	return 0;
}
