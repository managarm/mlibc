#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static sigjmp_buf env;

static void print_mask(const char *stage) {
	sigset_t current_set;

	if (sigprocmask(SIG_SETMASK, NULL, &current_set) == -1) {
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	printf("%s mask state:\n", stage);
	printf("\t- SIGUSR1 is %sBLOCKED\n", sigismember(&current_set, SIGUSR1) ? "" : "UN");
	printf("\t- SIGUSR2 is %sBLOCKED\n", sigismember(&current_set, SIGUSR2) ? "" : "UN");
}

int main(void) {
	sigset_t block_set;

	// Part 1: Test sigsetjmp(env, 1) so the mask should be saved and restored
	sigemptyset(&block_set);
	sigaddset(&block_set, SIGUSR1);

	if (sigprocmask(SIG_BLOCK, &block_set, NULL) == -1) {
		perror("sigprocmask init 1");
		return EXIT_FAILURE;
	}

	print_mask("Part 1 before sigsetjmp");

	if (sigsetjmp(env, 1) == 0) {
		sigset_t new_block;
		sigemptyset(&new_block);
		sigaddset(&new_block, SIGUSR2);
		sigprocmask(SIG_BLOCK, &new_block, NULL);

		sigset_t new_unblock;
		sigemptyset(&new_unblock);
		sigaddset(&new_unblock, SIGUSR1);
		sigprocmask(SIG_UNBLOCK, &new_unblock, NULL);

		print_mask("Part 1 post-modification");

		siglongjmp(env, 1);
	} else {
		print_mask("Part 1 after siglongjmp");

		sigset_t final_set;
		sigprocmask(SIG_SETMASK, NULL, &final_set);

		int sigusr1_blocked = sigismember(&final_set, SIGUSR1);
		int sigusr2_blocked = sigismember(&final_set, SIGUSR2);

		assert(sigusr1_blocked && !sigusr2_blocked);
	}

	// Part 2: Test sigsetjmp(env, 0) so the mask should not be saved/restored
	// Start with SIGUSR1 blocked and SIGUSR2 unblocked
	sigemptyset(&block_set);
	sigaddset(&block_set, SIGUSR1);
	sigset_t unblock_set;
	sigemptyset(&unblock_set);
	sigaddset(&unblock_set, SIGUSR2);
	sigprocmask(SIG_BLOCK, &block_set, NULL);
	sigprocmask(SIG_UNBLOCK, &unblock_set, NULL);

	print_mask("Part 2 before sigsetjmp");

	if (sigsetjmp(env, 0) == 0) {
		sigset_t new_block;
		sigemptyset(&new_block);
		sigaddset(&new_block, SIGUSR2);
		sigprocmask(SIG_BLOCK, &new_block, NULL);

		sigset_t new_unblock;
		sigemptyset(&new_unblock);
		sigaddset(&new_unblock, SIGUSR1);
		sigprocmask(SIG_UNBLOCK, &new_unblock, NULL);

		print_mask("Part 2 post-modification");

		siglongjmp(env, 1);
	} else {
		print_mask("Part 2 after siglongjmp");

		sigset_t final_set;
		sigprocmask(SIG_SETMASK, NULL, &final_set);

		int sigusr1_blocked = sigismember(&final_set, SIGUSR1);
		int sigusr2_blocked = sigismember(&final_set, SIGUSR2);

		assert(!sigusr1_blocked && sigusr2_blocked);
	}

	// Part 3: Test setjmp(env) so the mask should not be saved/restored
	// Start with SIGUSR1 blocked and SIGUSR2 unblocked
	memset(&env, 0xFF, sizeof(env));
	sigemptyset(&block_set);
	sigaddset(&block_set, SIGUSR1);
	sigemptyset(&unblock_set);
	sigaddset(&unblock_set, SIGUSR2);
	sigprocmask(SIG_BLOCK, &block_set, NULL);
	sigprocmask(SIG_UNBLOCK, &unblock_set, NULL);

	print_mask("Part 3 before setjmp");

	if (setjmp(env) == 0) {
		sigset_t new_block;
		sigemptyset(&new_block);
		sigaddset(&new_block, SIGUSR2);
		sigprocmask(SIG_BLOCK, &new_block, NULL);

		sigset_t new_unblock;
		sigemptyset(&new_unblock);
		sigaddset(&new_unblock, SIGUSR1);
		sigprocmask(SIG_UNBLOCK, &new_unblock, NULL);

		print_mask("Part 3 post-modification");

		siglongjmp(env, 1);
	} else {
		print_mask("Part 3 after siglongjmp");

		sigset_t final_set;
		sigprocmask(SIG_SETMASK, NULL, &final_set);

		int sigusr1_blocked = sigismember(&final_set, SIGUSR1);
		int sigusr2_blocked = sigismember(&final_set, SIGUSR2);

		assert(!sigusr1_blocked && sigusr2_blocked);
	}

	return EXIT_SUCCESS;
}
