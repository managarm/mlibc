#include <bits/ensure.h>
#include <utmp.h>

void setutent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct utmp *getutent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void endutent(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct utmp *pututline(const struct utmp *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct utmp *getutline(const struct utmp *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
