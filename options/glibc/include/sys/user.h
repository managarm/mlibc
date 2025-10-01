#ifndef _SYS_USER_H
#define _SYS_USER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__x86_64__)

struct user_fpregs_struct {
	uint16_t cwd, swd, ftw, fop;
	uint64_t rip, rdp;
	uint32_t mxcsr, mxcr_mask;
	uint32_t st_space[32], xmm_space[64], padding[24];
};

struct user_regs_struct {
	unsigned long r15, r14, r13, r12, rbp, rbx, r11, r10, r9, r8;
	unsigned long rax, rcx, rdx, rsi, rdi, orig_rax, rip;
	unsigned long cs, eflags, rsp, ss, fs_base, gs_base, ds, es, fs, gs;
};

struct user {
	struct user_regs_struct regs;
	int u_fpvalid;
	struct user_fpregs_struct i387;
	unsigned long u_tsize;
	unsigned long u_dsize;
	unsigned long u_ssize;
	unsigned long start_code;
	unsigned long start_stack;
	long signal;
	int reserved;
	struct user_regs_struct	*u_ar0;
	struct user_fpregs_struct *u_fpstate;
	unsigned long magic;
	char u_comm[32];
	unsigned long u_debugreg[8];
};

#elif defined(__i386__)

struct user_fpregs_struct {
	long int cwd, swd, twd, fip, fcs, foo, fos;
	long int st_space[20];
};

struct user_fpxregs_struct {
	unsigned short int cwd, swd, twd, fop;
	long int fip, fcs, foo, fos;
	long int mxcsr;
	long int reserved;
	long int st_space[32];
	long int xmm_space[32];
	long int padding[56];
};

struct user_regs_struct {
	long int ebx;
	long int ecx;
	long int edx;
	long int esi;
	long int edi;
	long int ebp;
	long int eax;
	long int xds;
	long int xes;
	long int xfs;
	long int xgs;
	long int orig_eax;
	long int eip;
	long int xcs;
	long int eflags;
	long int esp;
	long int xss;
};

struct user {
	struct user_regs_struct	regs;
	int u_fpvalid;
	struct user_fpregs_struct i387;
	unsigned long int u_tsize;
	unsigned long int u_dsize;
	unsigned long int u_ssize;
	unsigned long int start_code;
	unsigned long int start_stack;
	long int signal;
	int reserved;
	struct user_regs_struct *u_ar0;
	struct user_fpregs_struct *u_fpstate;
	unsigned long int magic;
	char u_comm[32];
	int u_debugreg[8];
};

#elif defined(__aarch64__)

struct user_regs_struct {
	unsigned long long regs[31];
	unsigned long long sp;
	unsigned long long pc;
	unsigned long long pstate;
};

struct user_fpsimd_struct {
	__uint128_t vregs[32];
	unsigned int fpsr;
	unsigned int fpcr;
};

#elif defined(__loongarch64)

struct user_regs_struct {
	uint64_t regs[32];
	uint64_t orig_a0;
	uint64_t csr_era;
	uint64_t csr_badv;
	uint64_t reserved[10];
};

struct user_fp_struct {
	uint64_t fpr[32];
	uint64_t fcc;
	uint32_t fcsr;
};

#elif defined(__m68k__)

struct user_m68kfp_struct {
	unsigned long fpregs[8 * 3];
	unsigned long fpcntl[3];
};

struct user_regs_struct {
	long d1, d2, d3, d4, d5, d6, d7;
	long a0, a1, a2, a3, a4, a5, a6;
	long d0;
	long usp;
	long orig_d0;
	short stkadj;
	short sr;
	long pc;
	short fmtvec;
	short __fill;
};

struct user {
	struct user_regs_struct regs;
	int u_fpvalid;
	struct user_m68kfp_struct m68kfp;
	unsigned long int u_tsize;
	unsigned long int u_dsize;
	unsigned long int u_ssize;
	unsigned long start_code;
	unsigned long start_stack;
	long int signal;
	int reserved;
	unsigned long u_ar0;
	struct user_m68kfp_struct *u_fpstate;
	unsigned long magic;
	char u_comm[32];
};

#elif defined(__riscv)

/* thankfully, nothing to do here. */

#else

#warning "<sys/user.h> unimplemented for this architecture!"

#endif

#ifdef __cplusplus
}
#endif

#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))

#endif
