#include <assert.h>
#include <execinfo.h>
#include <stdio.h>
#include <unistd.h>

void func() {
	void *callstack[128];
	int frames = backtrace(callstack, 128);
	fprintf(stderr, "backtrace returns %d frames:\n", frames);
	// we expect at least 3 frames: _start, main and func
	assert(frames >= 3);

	backtrace_symbols_fd(callstack, frames, STDERR_FILENO);
	return;
}

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

#if !defined(__m68k__)
	func();
#endif

	return 0;
}
