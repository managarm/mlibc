#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>

static void noop(int x) {
	(void)x;
}

int main() {
	signal(SIGUSR1, &noop);

	int pid;
	switch(pid = fork()) {
	case -1:
		perror("fork");
		abort();
	case 0:
		pause();
		assert(errno == EINTR);
		return 0;
	default:
		while (1) {
			usleep(100);
			kill(pid, SIGUSR1);
			usleep(100);
			int status;

			errno = 0;
			if (waitpid(-1, &status, WNOHANG) <= 0) {
				if (errno && errno != EAGAIN) {
					perror("wait");
					kill(pid, SIGKILL);
				}
				continue;
			}

			if (!WIFEXITED(status)) {
				printf("wait returned %x\n", status);
				abort();
			}

			return WEXITSTATUS(status);
		}
	}
}
