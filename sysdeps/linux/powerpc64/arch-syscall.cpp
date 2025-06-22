#include <bits/syscall.h>
#include <sys/syscall.h>

using sc_word_t = __sc_word_t;

sc_word_t transform_ret(sc_word_t ret) {
	// reading cr0.SO
	sc_word_t is_err;
	asm volatile("mfcr %0" : "=r"(is_err));
	is_err = (is_err >> 28) & 0b1;

	// branchless negation
	ret = (ret ^ -is_err) + is_err;
	return ret;
}

sc_word_t __do_syscall0(long sc) {
	register int sc_reg asm("r0") = sc;
	register sc_word_t ret asm("r3");
	asm volatile("sc" : "=r"(ret) : "r"(sc_reg) : "memory");
	return transform_ret(ret);
}

sc_word_t __do_syscall1(long sc, sc_word_t arg1) {
	register int sc_reg asm("r0") = sc;
	register sc_word_t arg1_reg asm("r3") = arg1;
	register sc_word_t ret asm("r3");
	asm volatile("sc" : "=r"(ret) : "r"(sc_reg), "r"(arg1_reg) : "memory");
	return transform_ret(ret);
}

sc_word_t __do_syscall2(long sc, sc_word_t arg1, sc_word_t arg2) {
	register int sc_reg asm("r0") = sc;
	register sc_word_t arg1_reg asm("r3") = arg1;
	register sc_word_t arg2_reg asm("r4") = arg2;
	register sc_word_t ret asm("r3");
	asm volatile("sc" : "=r"(ret) : "r"(sc_reg), "r"(arg1_reg), "r"(arg2_reg) : "memory");
	return transform_ret(ret);
}

sc_word_t __do_syscall3(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
	register int sc_reg asm("r0") = sc;
	register sc_word_t arg1_reg asm("r3") = arg1;
	register sc_word_t arg2_reg asm("r4") = arg2;
	register sc_word_t arg3_reg asm("r5") = arg3;
	register sc_word_t ret asm("r3");
	asm volatile("sc"
	             : "=r"(ret)
	             : "r"(sc_reg), "r"(arg1_reg), "r"(arg2_reg), "r"(arg3_reg)
	             : "memory");
	return transform_ret(ret);
}

sc_word_t __do_syscall4(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4) {
	register int sc_reg asm("r0") = sc;
	register sc_word_t arg1_reg asm("r3") = arg1;
	register sc_word_t arg2_reg asm("r4") = arg2;
	register sc_word_t arg3_reg asm("r5") = arg3;
	register sc_word_t arg4_reg asm("r6") = arg4;
	register sc_word_t ret asm("r3");
	asm volatile("sc"
	             : "=r"(ret)
	             : "r"(sc_reg), "r"(arg1_reg), "r"(arg2_reg), "r"(arg3_reg), "r"(arg4_reg)
	             : "memory");
	return transform_ret(ret);
}

sc_word_t __do_syscall5(
    long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3, sc_word_t arg4, sc_word_t arg5
) {
	register int sc_reg asm("r0") = sc;
	register sc_word_t arg1_reg asm("r3") = arg1;
	register sc_word_t arg2_reg asm("r4") = arg2;
	register sc_word_t arg3_reg asm("r5") = arg3;
	register sc_word_t arg4_reg asm("r6") = arg4;
	register sc_word_t arg5_reg asm("r7") = arg5;
	register sc_word_t ret asm("r3");
	asm volatile(
	    "sc"
	    : "=r"(ret)
	    : "r"(sc_reg), "r"(arg1_reg), "r"(arg2_reg), "r"(arg3_reg), "r"(arg4_reg), "r"(arg5_reg)
	    : "memory"
	);
	return transform_ret(ret);
}

sc_word_t __do_syscall6(
    long sc,
    sc_word_t arg1,
    sc_word_t arg2,
    sc_word_t arg3,
    sc_word_t arg4,
    sc_word_t arg5,
    sc_word_t arg6
) {
	register int sc_reg asm("r0") = sc;
	register sc_word_t arg1_reg asm("r3") = arg1;
	register sc_word_t arg2_reg asm("r4") = arg2;
	register sc_word_t arg3_reg asm("r5") = arg3;
	register sc_word_t arg4_reg asm("r6") = arg4;
	register sc_word_t arg5_reg asm("r7") = arg5;
	register sc_word_t arg6_reg asm("r8") = arg6;
	register sc_word_t ret asm("r3");
	asm volatile("sc"
	             : "=r"(ret)
	             : "r"(sc_reg),
	               "r"(arg1_reg),
	               "r"(arg2_reg),
	               "r"(arg3_reg),
	               "r"(arg4_reg),
	               "r"(arg5_reg),
	               "r"(arg6_reg)
	             : "memory");
	return transform_ret(ret);
}
