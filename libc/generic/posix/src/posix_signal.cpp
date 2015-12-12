
#include <signal.h>

#include <mlibc/ensure.h>

int kill(pid_t pid, int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

