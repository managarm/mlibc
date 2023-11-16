#include <sys/syscall.h>
#include <errno.h>
#include <sys/ptrace.h>

int ptrace(int request, pid_t pid, void *addr, void *data) {
	int ret;
	SYSCALL4(SYSCALL_PTRACE, request, pid, addr, data);
	return ret;
}
