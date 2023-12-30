#include <bits/syscall.h>
#include <sys/syscall.h>

using sc_word_t = __sc_word_t;

// Note: ebx is used for PIC (it holds a reference to the GOT), so we can't clobber it with gcc apparently,
// and also need to make sure to restore it after a syscall

sc_word_t __do_syscall0(long sc) {
	sc_word_t ret;
	asm volatile("int $0x80" : "=a"(ret) : "a"(sc) : "memory");
	return ret;
}

sc_word_t __do_syscall1(long sc, sc_word_t arg1) {
	sc_word_t ret;
	asm volatile("xchg %%ebx, %%edi;"
		"int $0x80;"
		"xchg %%edi, %%ebx;"
		: "=a"(ret)
		: "a"(sc), "D"(arg1)
		: "memory");
	return ret;
}

sc_word_t __do_syscall2(long sc, sc_word_t arg1, sc_word_t arg2) {
	sc_word_t ret;
	asm volatile("xchg %%ebx, %%edi;"
		"int $0x80;"
		"xchg %%edi, %%ebx;"
		: "=a"(ret)
		: "a"(sc), "D"(arg1), "c"(arg2)
		: "memory");
	return ret;
}

sc_word_t __do_syscall3(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
	sc_word_t ret;
	asm volatile("xchg %%ebx, %%edi;"
		"int $0x80;"
		"xchg %%edi, %%ebx;"
		: "=a"(ret)
		: "a"(sc), "D"(arg1), "c"(arg2), "d"(arg3)
		: "memory");
	return ret;
}

sc_word_t __do_syscall4(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4) {
	sc_word_t ret;
	asm volatile("xchg %%ebx, %%edi;"
		"int $0x80;"
		"xchg %%edi, %%ebx;"
		: "=a"(ret)
		: "a"(sc), "D"(arg1), "c"(arg2), "d"(arg3), "S"(arg4)
		: "memory");
	return ret;
}

sc_word_t __do_syscall5(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4,
		sc_word_t arg5) {
	sc_word_t ret;
	asm volatile("pushl %2;"
		"push %%ebx;"
		"mov 4(%%esp), %%ebx;"
		"int $0x80;"
		"pop %%ebx;"
		"add $4, %%esp;"
		: "=a"(ret)
		: "a"(sc), "g"(arg1), "c"(arg2), "d"(arg3), "S"(arg4), "D"(arg5)
		: "memory");
	return ret;
}

sc_word_t __do_syscall6(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4,
		sc_word_t arg5, sc_word_t arg6) {
	sc_word_t ret;
	sc_word_t a1a6[2] = { arg1, arg6 };
	asm volatile ("pushl %1;"
		"push %%ebx;"
		"push %%ebp;"
		"mov 8(%%esp),%%ebx;"
		"mov 4(%%ebx),%%ebp;"
		"mov (%%ebx),%%ebx;"
		"int $0x80;"
		"pop %%ebp;"
		"pop %%ebx;"
		"add $4,%%esp;"
		: "=a"(ret) : "g"(&a1a6), "a"(sc), "c"(arg2), "d"(arg3), "S"(arg4), "D"(arg5) : "memory");
	return ret;
}
