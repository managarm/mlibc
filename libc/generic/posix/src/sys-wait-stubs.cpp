
#include <sys/wait.h>

#include <mlibc/ensure.h>

pid_t wait(int *status) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// waitpid() is provided by the platform

