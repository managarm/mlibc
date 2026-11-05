#include <assert.h>
#include <dlfcn.h>
#include <stddef.h>

#ifdef USE_HOST_LIBC
#define PIE "native-pie"
#else
#define PIE "pie"
#endif

int main() {
	// Position-independent executables (DF_1_PIE) cannot be loaded with dlopen().
	assert(dlopen(PIE, RTLD_NOW) == NULL);
	assert(dlerror() != NULL);
	return 0;
}
