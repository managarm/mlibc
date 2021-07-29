#include <assert.h>
#include <pthread.h>
#include <signal.h>

_Atomic int handler_ready = 0;
_Atomic int thread_signal_ran = 0;

static void sig_handler(int sig, siginfo_t *info, void *ctx) {
	(void)sig;
	(void)info;
	(void)ctx;

	thread_signal_ran = 1;
}

static void *worker(void *arg) {
	(void)arg;

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = sig_handler;
	sa.sa_flags = SA_SIGINFO;
	assert(!sigaction(SIGUSR1, &sa, NULL));

	handler_ready = 1;

	while (!thread_signal_ran)
		;

	return NULL;
}

int main() {
	pthread_t thread;
	assert(!pthread_create(&thread, NULL, &worker, NULL));

	while (!handler_ready)
		;

	assert(!pthread_kill(thread, SIGUSR1));
	assert(!pthread_join(thread, NULL));

	assert(thread_signal_ran);

	return 0;
}
