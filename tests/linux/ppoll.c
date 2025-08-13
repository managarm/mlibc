#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void sig_handler(int sig) {
	(void)sig;
}

int main() {
	int fds[2];
	int ret = pipe(fds);
	assert(ret == 0);

	struct pollfd poll_fds[1];
	poll_fds[0].fd = fds[0];
	poll_fds[0].events = POLLIN;

	struct timespec timeout;
	timeout.tv_sec = 0;
	timeout.tv_nsec = 1000000; // 1 ms

	// Test that ppoll times out correctly.
	ret = ppoll(poll_fds, 1, &timeout, NULL);
	assert(ret == 0);

	// Test that ppoll returns 1 if there is an event.
	char buf[1];
	ret = write(fds[1], "a", 1);
	assert(ret == 1);

	ret = ppoll(poll_fds, 1, &timeout, NULL);
	assert(ret == 1);
	assert(poll_fds[0].revents & POLLIN);

	// Test that ppoll returns 0 if there are no events and timeout is 0.
	ret = read(fds[0], buf, 1);
	assert(ret == 1);

	timeout.tv_nsec = 0;
	ret = ppoll(poll_fds, 1, &timeout, NULL);
	assert(ret == 0);

	// Test that ppoll returns immediately if there is an event and timeout is 0.
	ret = write(fds[1], "a", 1);
	assert(ret == 1);

	ret = ppoll(poll_fds, 1, &timeout, NULL);
	assert(ret == 1);
	assert(poll_fds[0].revents & POLLIN);

	char recvbuf[2];
	ret = read(fds[0], &recvbuf, 2);
	assert(ret == 1);

	// Test that ppoll is interrupted by a signal.
	struct sigaction sa;
	sa.sa_handler = sig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	ret = sigaction(SIGUSR1, &sa, NULL);
	assert(ret == 0);

	pid_t pid = fork();
	assert(pid >= 0);

	if (pid == 0) {
		// Child process.
		usleep(100000); // 100 ms
		ret = kill(getppid(), SIGUSR1);
		assert(ret == 0);
		exit(0);
	} else {
		// Parent process.
		timeout.tv_sec = 1;
		timeout.tv_nsec = 0;
		ret = ppoll(poll_fds, 1, &timeout, NULL);
		assert(ret == -1);
		assert(errno == EINTR);

		int status;
		ret = waitpid(pid, &status, 0);
		assert(ret == pid);
		assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
	}

	close(fds[0]);
	close(fds[1]);

	return 0;
}
