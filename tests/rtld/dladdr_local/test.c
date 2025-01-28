#include <dlfcn.h>
#include <assert.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#else
#define LIBFOO "libfoo.so"
#endif

int main() {
	void *foo_handle = dlopen(LIBFOO, RTLD_LOCAL | RTLD_NOW);
	assert(foo_handle);
	
	char *foo_global = (char *)dlsym(foo_handle, "foo_global");
	assert(foo_global);
	
	Dl_info info;
	assert(dladdr((const void *)foo_global, &info) != 0);
	
	assert(dlclose(foo_handle) == 0);

	return 0;
}
