#include <stddef.h>
#include <stdlib.h>

#if !USE_HOST_LIBC && !USE_CROSS_LIBC
void* operator new(size_t size) {
	void* p = malloc(size);
	if (!p)
		abort();
	return p;
}

void operator delete(void* p) noexcept {
	free(p);
}

void operator delete(void* p, size_t) noexcept {
	free(p);
}
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC
