#include <bits/ensure.h>
#include <utmpx.h>

void updwtmpx(const char *, const struct utmpx *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void endutxent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void setutxent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct utmpx *getutxent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct utmpx *pututxline(const struct utmpx *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int utmpxname(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
