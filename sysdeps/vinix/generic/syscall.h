#include <stdint.h>

struct __syscall_ret {
	uint64_t ret;
	uint64_t errno;
};

__attribute__((naked))
static __syscall_ret __syscall(int number, ...) {
	asm (
		"mov %rcx, %r10\n\t"
		"syscall\n\t"
		"ret"
	);
	(void)number;
}
