#include <bits/ensure.h>
#include <sys/quota.h>

int quotactl(int, const char *, int, caddr_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
