#include <bits/ensure.h>
#include <utmpx.h>

void updwtmpx(const char *, const struct utmpx *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
