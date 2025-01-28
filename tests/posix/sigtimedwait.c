#include <stdio.h>

#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

pid_t parent;
pid_t child;

int fds[2];

void parent_fn() {
	int res;

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR2);
	sigprocmask(SIG_BLOCK, &set, NULL);

	res = write(fds[1], "!", 1);
	assert(res == 1);

	siginfo_t info;
	res = sigwaitinfo(&set, &info);
	assert(res == SIGUSR2);
	assert(info.si_signo == SIGUSR2);

	res = write(fds[1], "!", 1);
	assert(res == 1);

	// XXX: This may not be long enough to get scheduled
	struct timespec tout = {1, 0};
	res = sigtimedwait(&set, &info, &tout);
	assert(res == SIGUSR2);
	assert(info.si_signo == SIGUSR2);

	res = write(fds[1], "!", 1);
	assert(res == 1);

	int sig;
	res = sigwait(&set, &sig);
	assert(res == 0);
	assert(sig == SIGUSR2);

	res = write(fds[1], "!", 1);
	assert(res == 1);

	res = sigtimedwait(&set, &info, &tout);
	assert(res < 0);
	assert(errno == EAGAIN);

	int wsts;
	res = waitpid(child, &wsts, 0);
	assert(res >= 0);
}

void child_fn() {
	int res;
	char c;

	res = read(fds[0], &c, 1);
	assert(res == 1);
	kill(parent, SIGUSR2);
	res = read(fds[0], &c, 1);
	assert(res == 1);
	kill(parent, SIGUSR2);
	res = read(fds[0], &c, 1);
	assert(res == 1);
	kill(parent, SIGUSR2);
	res = read(fds[0], &c, 1);
	assert(res == 1);
}

int main() {
	int res;

	parent = getpid();
	assert(parent > 0);

	res = pipe(fds);
	assert(res == 0);

	child = fork();
	assert(child >= 0);
	if (child)
		parent_fn();
	else
		child_fn();

	return 0;
}
