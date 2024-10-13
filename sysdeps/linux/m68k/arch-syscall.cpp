#include <sys/syscall.h>
#include <bits/syscall.h>

using sc_word_t = __sc_word_t;

sc_word_t __do_syscall0(long sc) {
	register int sc_reg asm("d0") = sc;
	register sc_word_t ret asm("d0");
	asm volatile ("trap #0" : "=r"(ret) : "r"(sc_reg) : "memory");
	return ret;
}

sc_word_t __do_syscall1(long sc,
		sc_word_t arg1) {
	register int sc_reg asm("d0") = sc;
	register sc_word_t arg1_reg asm("d1") = arg1;
	register sc_word_t ret asm("d0");
	asm volatile ("trap #0" : "=r"(ret) :
			"r"(sc_reg),
			"r"(arg1_reg)
			: "memory");
	return ret;
}

sc_word_t __do_syscall2(long sc,
		sc_word_t arg1, sc_word_t arg2) {
	register int sc_reg asm("d0") = sc;
	register sc_word_t arg1_reg asm("d1") = arg1;
	register sc_word_t arg2_reg asm("d2") = arg2;
	register sc_word_t ret asm("d0");
	asm volatile ("trap #0" : "=r"(ret) :
			"r"(sc_reg),
			"r"(arg1_reg),
			"r"(arg2_reg)
			: "memory");
	return ret;
}

sc_word_t __do_syscall3(long sc,
		sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
	register int sc_reg asm("d0") = sc;
	register sc_word_t arg1_reg asm("d1") = arg1;
	register sc_word_t arg2_reg asm("d2") = arg2;
	register sc_word_t arg3_reg asm("d3") = arg3;
	register sc_word_t ret asm("d0");
	asm volatile ("trap #0" : "=r"(ret) :
			"r"(sc_reg),
			"r"(arg1_reg),
			"r"(arg2_reg),
			"r"(arg3_reg)
			: "memory");
	return ret;
}

sc_word_t __do_syscall4(long sc,
		sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
		sc_word_t arg4) {
	register int sc_reg asm("d0") = sc;
	register sc_word_t arg1_reg asm("d1") = arg1;
	register sc_word_t arg2_reg asm("d2") = arg2;
	register sc_word_t arg3_reg asm("d3") = arg3;
	register sc_word_t arg4_reg asm("d4") = arg4;
	register sc_word_t ret asm("d0");
	asm volatile ("trap #0" : "=r"(ret) :
			"r"(sc_reg),
			"r"(arg1_reg),
			"r"(arg2_reg),
			"r"(arg3_reg),
			"r"(arg4_reg)
			: "memory");
	return ret;
}

sc_word_t __do_syscall5(long sc,
		sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
		sc_word_t arg4, sc_word_t arg5) {
	register int sc_reg asm("d0") = sc;
	register sc_word_t arg1_reg asm("d1") = arg1;
	register sc_word_t arg2_reg asm("d2") = arg2;
	register sc_word_t arg3_reg asm("d3") = arg3;
	register sc_word_t arg4_reg asm("d4") = arg4;
	register sc_word_t arg5_reg asm("d5") = arg5;
	register sc_word_t ret asm("d0");
	asm volatile ("trap #0" : "=r"(ret) :
			"r"(sc_reg),
			"r"(arg1_reg),
			"r"(arg2_reg),
			"r"(arg3_reg),
			"r"(arg4_reg),
			"r"(arg5_reg)
			: "memory");
	return ret;
}

sc_word_t __do_syscall6(long sc,
		sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
		sc_word_t arg4, sc_word_t arg5, sc_word_t arg6) {
	register int sc_reg asm("d0") = sc;
	register sc_word_t arg1_reg asm("d1") = arg1;
	register sc_word_t arg2_reg asm("d2") = arg2;
	register sc_word_t arg3_reg asm("d3") = arg3;
	register sc_word_t arg4_reg asm("d4") = arg4;
	register sc_word_t arg5_reg asm("d5") = arg5;
	register sc_word_t arg6_reg asm("a0") = arg6;
	register sc_word_t ret asm("d0");
	asm volatile ("trap #0" : "=r"(ret) :
			"r"(sc_reg),
			"r"(arg1_reg),
			"r"(arg2_reg),
			"r"(arg3_reg),
			"r"(arg4_reg),
			"r"(arg5_reg),
			"r"(arg6_reg)
			: "memory"
			);
	return ret;
}
