#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#if defined(USE_HOST_LIBC) || defined(USE_CROSS_LIBC)
#include <sys/syscall.h>

int rt_tgsigqueueinfo(pid_t tgid, pid_t tid, int sig, siginfo_t *uinfo) {
	return syscall(SYS_rt_tgsigqueueinfo, tgid, tid, sig, uinfo);
}
#else
#include <mlibc/wrappers.h>
#endif

int target_tid = 0;
volatile sig_atomic_t caught = 0;

void handler(int sig, siginfo_t *info, void *ucontext) {
	(void) ucontext;
	assert(sig == SIGUSR1);
	assert(gettid() == target_tid);
	assert(info->si_value.sival_int == 5678);
	caught = 1;
}

void *thread_func(void *arg) {
	(void) arg;
	sigset_t mask, oldmask;

	sigemptyset(&mask);
	sigaddset(&mask, SIGUSR1);
	pthread_sigmask(SIG_BLOCK, &mask, &oldmask);

	__atomic_store_n(&target_tid, gettid(), __ATOMIC_RELAXED);

	sigset_t wait_mask;
	sigemptyset(&wait_mask);

	while (!caught)
		sigsuspend(&wait_mask);

	return NULL;
}

int main() {
	struct sigaction sa = {0};
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);

    pthread_t thread;
    if (pthread_create(&thread, NULL, thread_func, NULL) != 0) {
		perror("pthread_create");
        return 1;
    }

    while (__atomic_load_n(&target_tid, __ATOMIC_RELAXED) == 0)
        usleep(100);

    siginfo_t info = {
        .si_signo = SIGUSR1,
        .si_code = SI_QUEUE,
        .si_value.sival_int = 5678
    };

    if (rt_tgsigqueueinfo(getpid(), __atomic_load_n(&target_tid, __ATOMIC_RELAXED), SIGUSR1, &info) != 0) {
        perror("rt_tgsigqueueinfo");
        return 1;
    }

    pthread_join(thread, NULL);
    return 0;
}
