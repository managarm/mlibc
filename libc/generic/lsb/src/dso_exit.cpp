
#include <mlibc/ensure.h>

extern "C" int __cxa_atexit(void (*handler)(void *), void *argument, void *dso) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

