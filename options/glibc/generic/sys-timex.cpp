#include <sys/timex.h>
#include <bits/ensure.h>

int clock_adjtime(clockid_t, struct timex *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
