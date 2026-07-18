#include <roxy/syscall.h>

extern "C" roxy_syscall_word_t roxy_syscall3(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second,
	roxy_syscall_word_t third
) {
	roxy_syscall_word_t result;

	asm volatile("syscall"
	             : "=a"(result)
	             : "a"(number), "D"(first), "S"(second), "d"(third)
	             : "rcx", "r11", "memory");

	return result;
}
