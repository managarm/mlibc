#include <assert.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>

volatile sig_atomic_t receivedSigno = 0;

void handler(int signo, siginfo_t *info, void *uctx) {
	(void) uctx;
	receivedSigno = signo;
	assert(info);
	assert(info->si_signo == SIGUSR1);
	assert(info->si_code == SI_QUEUE);
	assert(info->si_pid == getpid());
	assert(info->si_uid == getuid());
	assert(info->si_value.sival_ptr == (void *) (uintptr_t) 0xDEADBEEF);
}

int main(void) {
	struct sigaction sa = { .sa_sigaction = handler, .sa_flags = SA_SIGINFO };
	int ret = sigaction(SIGUSR1, &sa, NULL);
	assert(!ret);

	union sigval sv;
	sv.sival_ptr = (void *) (uintptr_t) 0xDEADBEEF;
	ret = sigqueue(getpid(), SIGUSR1, sv);
	assert(!ret);
	assert(receivedSigno == SIGUSR1);

	return 0;
}
