#include <bits/ensure.h>
#include <malloc.h>

size_t malloc_usable_size(void *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
