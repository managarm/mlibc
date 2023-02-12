#include <bits/ensure.h>
#include <sys/personality.h>

int personality(unsigned long) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
