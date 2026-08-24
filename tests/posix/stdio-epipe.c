#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
	int input_fds[2];
	assert(pipe(input_fds) == 0);
	FILE *input = fdopen(input_fds[0], "r");
	assert(input);
	assert(close(input_fds[1]) == 0);

	// A failed seek on a pipe must not leak into a later I/O error.
	errno = 0;
	assert(fseek(input, 0, SEEK_CUR) == -1);
	assert(errno == ESPIPE);
	assert(!ferror(input));
	assert(fclose(input) == 0);

#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
	int invalid_fds[2];
	assert(pipe(invalid_fds) == 0);
	FILE *invalid_stream = fdopen(invalid_fds[1], "w");
	assert(invalid_stream);
	assert(close(invalid_fds[0]) == 0);
	assert(close(invalid_fds[1]) == 0);
	errno = 0;
	assert(fputs("x", invalid_stream) == EOF);
	assert(errno == EBADF);
	assert(ferror(invalid_stream));
	assert(fclose(invalid_stream) == EOF);
#endif

	void (*old_sigpipe)(int) = signal(SIGPIPE, SIG_IGN);
	assert(old_sigpipe != SIG_ERR);

	int puts_fds[2];
	assert(pipe(puts_fds) == 0);
	assert(close(puts_fds[0]) == 0);
	assert(dup2(puts_fds[1], STDOUT_FILENO) == STDOUT_FILENO);
	assert(close(puts_fds[1]) == 0);
	assert(setvbuf(stdout, NULL, _IOLBF, 0) == 0);
	errno = 0;
	assert(puts("x") == EOF);
	assert(errno == EPIPE);
	assert(ferror(stdout));
	assert(freopen("/dev/null", "w", stdout));

	int close_fds[2];
	assert(pipe(close_fds) == 0);
	assert(close(close_fds[0]) == 0);
	FILE *close_stream = fdopen(close_fds[1], "w");
	assert(close_stream);
	assert(fputs("x", close_stream) != EOF);
	errno = 0;
	assert(fclose(close_stream) == EOF);
	assert(errno == EPIPE);

	int line_fds[2];
	assert(pipe(line_fds) == 0);
	assert(close(line_fds[0]) == 0);
	FILE *line_stream = fdopen(line_fds[1], "w");
	assert(line_stream);
	assert(setvbuf(line_stream, NULL, _IOLBF, 0) == 0);
	errno = 0;
	assert(fputs("x\n", line_stream) == EOF);
	assert(errno == EPIPE);
	assert(ferror(line_stream));
	errno = 0;
	int close_result = fclose(line_stream);
	assert(close_result == 0 || (close_result == EOF && errno == EPIPE));

	int fprintf_fds[2];
	assert(pipe(fprintf_fds) == 0);
	assert(close(fprintf_fds[0]) == 0);
	FILE *fprintf_stream = fdopen(fprintf_fds[1], "w");
	assert(fprintf_stream);
	assert(setvbuf(fprintf_stream, NULL, _IOLBF, 0) == 0);
	errno = 0;
	assert(fprintf(fprintf_stream, "x\n") < 0);
	assert(errno == EPIPE);
	assert(ferror(fprintf_stream));
	errno = 0;
	close_result = fclose(fprintf_stream);
	assert(close_result == 0 || (close_result == EOF && errno == EPIPE));

	int flush_fds[2];
	assert(pipe(flush_fds) == 0);
	assert(close(flush_fds[0]) == 0);
	FILE *flush_stream = fdopen(flush_fds[1], "w");
	assert(flush_stream);
	assert(fputs("x", flush_stream) != EOF);
	assert(fflush(flush_stream) == EOF);
	assert(errno == EPIPE);
	assert(ferror(flush_stream));
	// libc implementations differ on whether a subsequent fclose() retries
	// data from a failed fflush(). Both outcomes are valid here.
	errno = 0;
	close_result = fclose(flush_stream);
	assert(close_result == 0 || (close_result == EOF && errno == EPIPE));

	assert(signal(SIGPIPE, old_sigpipe) != SIG_ERR);
	return 0;
}
