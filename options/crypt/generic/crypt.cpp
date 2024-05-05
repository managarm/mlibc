#include <crypt.h>
#include <bits/ensure.h>

char *crypt(const char *, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
