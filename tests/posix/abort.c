#include <signal.h>
#include <assert.h>
#include <stdlib.h>

void handler(int sig, siginfo_t *info, void *ctx) {
	(void)sig;
	(void)info;
	(void)ctx;
}

int main() {
	struct sigaction sa;
	sa.sa_sigaction = handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);

	int ret = sigaction(SIGABRT, &sa, NULL);
	assert(!ret);

	abort();
}
