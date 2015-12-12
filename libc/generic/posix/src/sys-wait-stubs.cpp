
#include <sys/wait.h>

#include <mlibc/ensure.h>

pid_t wait(int *status) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
pid_t waitpid(pid_t pid, int *status, int flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

