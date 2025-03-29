#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/pidfd.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
	int child = fork();

	if(!child) {
		sleep(1);
		exit(42);
	}

	int pidfd = pidfd_open(child, 0);
	assert(pidfd > 0);

	int ret = lseek(pidfd, 0, SEEK_SET);
	assert(ret == -1);
	assert(errno == ESPIPE);

	pid_t outpid = pidfd_getpid(pidfd);
	assert(outpid == child);

	struct pollfd pollfd;
	pollfd.fd = pidfd;
	pollfd.events = POLLIN;

	int ready = poll(&pollfd, 1, 0);
	assert(ready == 0);

	ready = poll(&pollfd, 1, 2000);
	if(ready == -1) {
		perror("poll");
		exit(EXIT_FAILURE);
	}
	assert(ready == 1);
	assert(pollfd.revents == POLLIN);

	siginfo_t info = {};
	ret = waitid(P_PIDFD, pidfd, &info, WEXITED | WNOHANG);
	assert(ret == 0);
	assert(info.si_code == CLD_EXITED);
	assert(info.si_pid == child);
	assert(info.si_status == 42);

	close(pidfd);

	child = fork();

	if(!child) {
		sleep(10);
	}

	pidfd = pidfd_open(child, 0);
	assert(pidfd > 0);

	ret = pidfd_send_signal(pidfd, SIGKILL, NULL, 0);
	assert(ret == 0);

	memset(&info, 0, sizeof(info));
	ret = waitid(P_PIDFD, pidfd, &info, WEXITED);
	assert(ret == 0);
	assert(info.si_code == CLD_KILLED);
	assert(info.si_pid == child);
	assert(info.si_status == SIGKILL);

	return 0;
}
