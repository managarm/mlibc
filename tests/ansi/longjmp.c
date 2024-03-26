#include <stdio.h>
#include <setjmp.h>
#include <stdnoreturn.h>
#include <assert.h>

jmp_buf buf;

noreturn void do_jump(int arg) {
	longjmp(buf, 2 * arg);
}

int main(void) {
	volatile int times_called = 0;

	if (setjmp(buf) != 8) {
		do_jump(++times_called);
	}

	assert(times_called == 4);

	times_called = 0;
	int ret = setjmp(buf);
	assert(ret == times_called);

	if (!ret) {
		times_called = 1;
		longjmp(buf, 0);
	}

	return 0;
}
