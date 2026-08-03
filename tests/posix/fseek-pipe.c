#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
	int pipe_fds[2];
	assert(!pipe(pipe_fds));

	FILE *stream = fdopen(pipe_fds[1], "w");
	assert(stream);

	errno = 0;
	assert(fseek(stream, 0, SEEK_CUR) == -1);
	assert(errno == ESPIPE);
	assert(!ferror(stream));

	assert(!fclose(stream));
	assert(!close(pipe_fds[0]));
	return 0;
}
