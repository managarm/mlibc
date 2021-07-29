#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>

static jmp_buf env;

static void sig_handler(int sig, siginfo_t *info, void *ctx) {
	(void)sig;
	(void)info;
	(void)ctx;

	longjmp(env, 1);
}

int main() {
	if (setjmp(env)) {
		return 0;
	}

	stack_t ss;
	ss.ss_sp = malloc(SIGSTKSZ);
	ss.ss_size = SIGSTKSZ;
	ss.ss_flags = 0;

	assert(!sigaltstack(&ss, NULL));

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
	sa.sa_sigaction = sig_handler;

	assert(!sigaction(SIGSEGV, &sa, NULL));

	// This is used to trash the stack to ensure sigaltstack actually switched stacks.
#if defined(__x86_64__)
	asm volatile ("mov $0, %rsp\n\tpush $0");
#elif defined(__aarch64__)
	asm volatile ("mov sp, %0\n\tstp x0, x1, [sp, #-16]!" :: "r"(uint64_t{0}));
#else
#	error Unknown architecture
#endif
}
