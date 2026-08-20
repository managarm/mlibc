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

static int wait_for(pid_t pid) {
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

// Forks and checks that the handlers ran in the order POSIX asks for: the
// prepare ones in the reverse order of registration, the parent and child ones
// in the order they were registered.
static int fork_and_check(void) {
	prepare_order = 0;
	parent_order = 0;
	child_order = 0;

	pid_t pid = fork();
	assert(pid >= 0);

	if (!pid) {
		assert(child_order == 2);
		exit(0);
	}

	assert(prepare_order == 1);
	assert(parent_order == 2);

	return wait_for(pid);
}

static void *forking_thread(void *arg) {
	*(int *)arg = fork_and_check();
	return NULL;
}

int main() {
	assert(!pthread_atfork(prepare1, parent1, child1));
	assert(!pthread_atfork(prepare2, parent2, child2));

	int ret = fork_and_check();
	if (ret)
		return ret;

	// The handlers belong to the process, so they have to run for a fork from
	// a thread that did not register any of them either.
	pthread_t thread;
	assert(!pthread_create(&thread, NULL, forking_thread, &ret));
	assert(!pthread_join(thread, NULL));

	return ret;
}
