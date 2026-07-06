#include <assert.h>
#include <dlfcn.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#else
#define LIBFOO "libfoo.so"
#endif

int main(void) {
	void *handle = dlopen(LIBFOO, RTLD_NOW | RTLD_LOCAL);
	assert(handle);

	void *(*get_tls_pointer)(void) = dlsym(handle, "get_tls_pointer");
	void *(*get_object_pointer)(void) = dlsym(handle, "get_object_pointer");
	assert(get_tls_pointer);
	assert(get_object_pointer);
	assert(get_tls_pointer() == get_object_pointer());

	return 0;
}
