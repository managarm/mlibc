
#include <sys/wait.h>

#include <bits/ensure.h>

pid_t wait(int *status) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int waitid(idtype_t idtype, id_t id, siginfo_t *siginfo, int flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// waitpid() is provided by the platform

