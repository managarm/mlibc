
#include <sys/random.h>
#include <mlibc/ensure.h>

ssize_t getrandom(void *, size_t, unsigned int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

