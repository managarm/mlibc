#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

#if defined(USE_HOST_LIBC) || defined(USE_CROSS_LIBC)
#include <sys/syscall.h>

int rt_sigqueueinfo(pid_t pid, int sig, siginfo_t *uinfo) {
	return syscall(SYS_rt_sigqueueinfo, pid, sig, uinfo);
}
#else
#include <mlibc/wrappers.h>
#endif

void handler(int sig, siginfo_t *info, void *arg) {
	(void) arg;

	assert(sig == SIGUSR1);
	assert(info->si_value.sival_int == 1234);
	_exit(0);
}

int main() {
	struct sigaction sa = {0};
	sa.sa_sigaction = handler;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa, NULL);

	siginfo_t info = {0};
	info.si_signo = SIGUSR1;
	info.si_code = SI_QUEUE;
	info.si_value.sival_int = 1234;

	if (rt_sigqueueinfo(getpid(), SIGUSR1, &info) != 0)
		return 1;

	pause();
	return 1;
}
