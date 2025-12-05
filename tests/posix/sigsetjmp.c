#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Test Logic:
// 1. Block SIGUSR1.
// 2. Call sigsetjmp(env, 1) to save context + mask.
// 3. Change mask: Unblock SIGUSR1 and block SIGUSR2.
// 4. Call siglongjmp.
// 5. Verify that the mask was reverted (SIGUSR1 blocked, SIGUSR2 unblocked).

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

	sigemptyset(&block_set);
	sigaddset(&block_set, SIGUSR1);

	if (sigprocmask(SIG_BLOCK, &block_set, NULL) == -1) {
		perror("sigprocmask init");
		return EXIT_FAILURE;
	}

	print_mask("before sigsetjmp");

	if (sigsetjmp(env, 1) == 0) {
		sigset_t new_block;
		sigemptyset(&new_block);
		sigaddset(&new_block, SIGUSR2);
		sigprocmask(SIG_BLOCK, &new_block, NULL);

		sigset_t new_unblock;
		sigemptyset(&new_unblock);
		sigaddset(&new_unblock, SIGUSR1);
		sigprocmask(SIG_UNBLOCK, &new_unblock, NULL);

		print_mask("post-modification");

		siglongjmp(env, 1);
	} else {
		print_mask("after siglongjmp");

		sigset_t final_set;
		sigprocmask(SIG_SETMASK, NULL, &final_set);

		int sigusr1_blocked = sigismember(&final_set, SIGUSR1);
		int sigusr2_blocked = sigismember(&final_set, SIGUSR2);

		if (sigusr1_blocked && !sigusr2_blocked) {
			return EXIT_SUCCESS;
		} else {
			printf("Signal mask was not correctly restored.\n");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
