#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>

static jmp_buf env;
static char *sigStack;

static void sig_handler(int sig, siginfo_t *info, void *ctx) {
	(void)sig;
	(void)info;
	(void)ctx;

	longjmp(env, 1);
}

int main() {
	if (setjmp(env)) {
		free(sigStack);
		return 0;
	}

	sigStack = malloc(SIGSTKSZ);

	stack_t ss;
	ss.ss_sp = sigStack;
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
	asm volatile ("mov $0, %rsp\n"
			"\t" "push $0");
#elif defined(__i386__)
	asm volatile ("mov $0, %esp\n"
			"\t" "push $0");
#elif defined(__aarch64__)
	asm volatile ("mov sp, %0\n"
			"\t" "stp x0, x1, [sp, #-16]!" :: "r"((uint64_t)0));
#elif defined(__riscv) && __riscv_xlen == 64
	asm volatile ("li sp, 0\n"
			"\t" "sd zero, 0(sp)");
#elif defined (__m68k__)
	asm volatile ("move.l #0, %sp\n"
			"\t" "move.l #0, -(%sp)");
#elif defined(__loongarch64)
	asm volatile ("addi.d $sp, $r0, 0\n"
		"\t" "st.d $r0, $sp, 0");
#else
#	error Unknown architecture
#endif

	return 0;
}
