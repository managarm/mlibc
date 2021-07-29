#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

_Atomic int prepare_order = 0;
_Atomic int parent_order = 0;
_Atomic int child_order = 0;

static void prepare1() { prepare_order = 1; }
static void prepare2() { prepare_order = 2; }

static void parent1() { parent_order = 1; }
static void parent2() { parent_order = 2; }

static void child1() { child_order = 1; }
static void child2() { child_order = 2; }

int main() {
	assert(!pthread_atfork(prepare1, parent1, child1));
	assert(!pthread_atfork(prepare2, parent2, child2));

	pid_t pid = fork();
	assert(pid >= 0);

	if (!pid) {
		assert(child_order == 2);
		exit(0);
	} else {
		assert(prepare_order == 1);
		assert(parent_order == 2);

		while (1) {
			int status = 0;

			int ret = waitpid(pid, &status, 0);

			if (ret == -1 && errno == EINTR)
				continue;

			assert(ret > 0);

			if (WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT)
				return 1;

			return WEXITSTATUS(status);
		}
	}

	return 0;
}
