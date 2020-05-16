
#include <syslog.h>
#include <bits/ensure.h>

void closelog(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void openlog(const char *, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setlogmask(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void syslog(int, const char *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void vsyslog(int, const char *, va_list) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

