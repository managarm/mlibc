#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// Make sure files are flushed on process exit.

int main() {
	int pipefd[2];
	if (pipe(pipefd) < 0) {
		perror("pipe");
		return -1;
	}

	pid_t child = fork();
	if (child < 0) {
		perror("fork");
		return -1;
	}

	// This should sit in libc's buffer and be flushed on exit.
	const char *message = "hello world";

	if (child == 0) {
		// Child
		dup2(pipefd[1], STDOUT_FILENO);
		printf("%s", message);
		return 0;
	}

	// Parent
	wait(NULL);

	char buffer[64];
	ssize_t len = read(pipefd[0], buffer, sizeof(buffer));
	assert(len > 0);
	buffer[len] = 0;

	return strcmp(buffer, message) != 0;
}

