#include <execinfo.h>
#include <bits/ensure.h>

int backtrace(void **, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char **backtrace_symbols(void *const *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void backtrace_symbols_fd(void *const *, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
