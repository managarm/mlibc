#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void prevent_atforks(void){
	exit(1);
}

int main() {
	pid_t pid;
	int status;
	assert(!pthread_atfork(prevent_atforks, NULL, NULL));

	switch (pid = vfork()) {
	case -1:
		perror("vfork");
		abort();
	case 0:
		_exit(12);
	default: break;
	}

	assert(wait(&status) == pid);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 12);

	return 0;
}
