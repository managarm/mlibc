#include <bits/ensure.h>
#include <sys/timex.h>

int adjtimex(struct timex *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int clock_adjtime(clockid_t, struct timex *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int ntp_adjtime(struct timex *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
