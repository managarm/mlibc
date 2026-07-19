#include <roxy/syscall.h>

extern "C" roxy_syscall_word_t roxy_syscall1(
	long number,
	roxy_syscall_word_t first
) {
	roxy_syscall_word_t result;

	asm volatile("syscall"
	             : "=a"(result)
	             : "a"(number), "D"(first)
	             : "rcx", "r11", "memory");

	return result;
}

extern "C" roxy_syscall_word_t roxy_syscall2(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second
) {
	roxy_syscall_word_t result;

	asm volatile("syscall"
	             : "=a"(result)
	             : "a"(number), "D"(first), "S"(second)
	             : "rcx", "r11", "memory");

	return result;
}

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

extern "C" roxy_syscall_word_t roxy_syscall4(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second,
	roxy_syscall_word_t third,
	roxy_syscall_word_t fourth
) {
	register roxy_syscall_word_t fourth_register asm("r10") = fourth;
	roxy_syscall_word_t result;

	asm volatile("syscall"
	             : "=a"(result)
	             : "a"(number), "D"(first), "S"(second), "d"(third), "r"(fourth_register)
	             : "rcx", "r11", "memory");

	return result;
}

extern "C" roxy_syscall_word_t roxy_syscall5(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second,
	roxy_syscall_word_t third,
	roxy_syscall_word_t fourth,
	roxy_syscall_word_t fifth
) {
	register roxy_syscall_word_t fourth_register asm("r10") = fourth;
	register roxy_syscall_word_t fifth_register asm("r8") = fifth;
	roxy_syscall_word_t result;

	asm volatile("syscall"
	             : "=a"(result)
	             : "a"(number), "D"(first), "S"(second), "d"(third), "r"(fourth_register),
	               "r"(fifth_register)
	             : "rcx", "r11", "memory");

	return result;
}

extern "C" roxy_syscall_word_t roxy_syscall6(
	long number,
	roxy_syscall_word_t first,
	roxy_syscall_word_t second,
	roxy_syscall_word_t third,
	roxy_syscall_word_t fourth,
	roxy_syscall_word_t fifth,
	roxy_syscall_word_t sixth
) {
	register roxy_syscall_word_t fourth_register asm("r10") = fourth;
	register roxy_syscall_word_t fifth_register asm("r8") = fifth;
	register roxy_syscall_word_t sixth_register asm("r9") = sixth;
	roxy_syscall_word_t result;

	asm volatile("syscall"
	             : "=a"(result)
	             : "a"(number), "D"(first), "S"(second), "d"(third), "r"(fourth_register),
	               "r"(fifth_register), "r"(sixth_register)
	             : "rcx", "r11", "memory");

	return result;
}
