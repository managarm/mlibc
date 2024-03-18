#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

siginfo_t si;

int main() {
	int pid = fork();

	if(!pid)
		exit(69);

	int ret = waitid(P_ALL, 0, &si, WEXITED);
	assert(ret == 0);
	assert(si.si_pid == pid);
	assert(si.si_signo == SIGCHLD);
	assert(si.si_code == CLD_EXITED);

	return 0;
}
