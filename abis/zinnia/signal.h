#ifndef _ABIBITS_SIGNAL_H
#define _ABIBITS_SIGNAL_H

#include <mlibc-config.h>

#include <abi-bits/pid_t.h>
#include <abi-bits/sigevent.h>
#include <abi-bits/sigset_t.h>
#include <abi-bits/uid_t.h>
#include <bits/size_t.h>
#include <bits/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int si_signo;
	int si_code;
	int si_errno;
	pid_t si_pid;
	uid_t si_uid;
	void *si_addr;
	int si_status;
	union sigval si_value;
} siginfo_t;

typedef void (*__sighandler)(int);

#define SIG_ERR ((__sighandler)(void *)(-1))
#define SIG_DFL ((__sighandler)(void *)(0))
#define SIG_IGN ((__sighandler)(void *)(1))

#define SIGABRT 1
#define SIGALRM 2
#define SIGBUS 3
#define SIGCHLD 4
#define SIGCONT 5
#define SIGFPE 6
#define SIGHUP 7
#define SIGILL 8
#define SIGINT 9
#define SIGKILL 10
#define SIGPIPE 11
#define SIGQUIT 12
#define SIGSEGV 13
#define SIGSTOP 14
#define SIGTERM 15
#define SIGTSTP 16
#define SIGTTIN 17
#define SIGTTOU 18
#define SIGUSR1 19
#define SIGUSR2 20
#define SIGWINCH 21
#define SIGSYS 22
#define SIGUNUSED SIGSYS
#define SIGTRAP 23
#define SIGURG 24
#define SIGVTALRM 25
#define SIGXCPU 26
#define SIGXFSZ 27

#define SIGIO 28
#define SIGPOLL 29
#define SIGPROF 30
#define SIGPWR 31
#define SIGIOT 32
#define SIGCANCEL 33

#define SIGRTMIN 35
#define SIGRTMAX 64

/* SIGBUS */
#define BUS_ADRALN 1
#define BUS_ADRERR 2
#define BUS_OBJERR 3

/* SIGILL */
#define ILL_ILLOPC 1
#define ILL_ILLOPN 2
#define ILL_ILLADR 3
#define ILL_ILLTRP 4
#define ILL_PRVOPC 5
#define ILL_PRVREG 6
#define ILL_COPROC 7
#define ILL_BADSTK 8
#define ILL_BADIADDR 9

/* SIGSEGV */
#define SEGV_MAPERR 1
#define SEGV_ACCERR 2

#define SIG_BLOCK 1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 3

#define SA_NOCLDSTOP (1 << 0)
#define SA_ONSTACK (1 << 1)
#define SA_RESETHAND (1 << 2)
#define SA_RESTART (1 << 3)
#define SA_SIGINFO (1 << 4)
#define SA_NOCLDWAIT (1 << 5)
#define SA_NODEFER (1 << 6)

#define SA_NOMASK SA_NODEFER
#define SA_ONESHOT SA_RESETHAND

#if defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN
#if defined(__aarch64__)
#define MINSIGSTKSZ 5120
#define SIGSTKSZ 16384
#else
#define MINSIGSTKSZ 2048
#define SIGSTKSZ 8192
#endif

#define SS_ONSTACK 1
#define SS_DISABLE 2
#endif

typedef struct __stack {
	void *ss_sp;
	size_t ss_size;
	int ss_flags;
} stack_t;

#define SIGEV_NONE 1
#define SIGEV_SIGNAL 2
#define SIGEV_THREAD 3

#define SI_ASYNCNL (-60)
#define SI_TKILL (-6)
#define SI_SIGIO (-5)
#define SI_ASYNCIO (-4)
#define SI_MESGQ (-3)
#define SI_TIMER (-2)
#define SI_QUEUE (-1)
#define SI_USER 0
#define SI_KERNEL 128

#define _NSIG 65
#if defined(_DEFAULT_SOURCE)
#define NSIG _NSIG
#endif

#define CLD_EXITED 1
#define CLD_KILLED 2
#define CLD_DUMPED 3
#define CLD_TRAPPED 4
#define CLD_STOPPED 5
#define CLD_CONTINUED 6

struct sigaction {
	union {
		void (*sa_handler)(int);
		void (*sa_sigaction)(int, siginfo_t *, void *);
	} __sa_handler;
	int sa_flags;
	void (*sa_restorer)(void);
	sigset_t sa_mask;
};

#define sa_handler __sa_handler.sa_handler
#define sa_sigaction __sa_handler.sa_sigaction

#ifdef _DEFAULT_SOURCE
#define __pollution(n) n
#else
#define __pollution(n) __##n
#endif

#if defined(__x86_64__)

#if defined(_GNU_SOURCE) || defined(__MLIBC_BUILDING_MLIBC)
#define REG_R8 0
#define REG_R9 1
#define REG_R10 2
#define REG_R11 3
#define REG_R12 4
#define REG_R13 5
#define REG_R14 6
#define REG_R15 7
#define REG_RDI 8
#define REG_RSI 9
#define REG_RBP 10
#define REG_RBX 11
#define REG_RDX 12
#define REG_RAX 13
#define REG_RCX 14
#define REG_RSP 15
#define REG_RIP 16
#define REG_EFL 17
#define REG_CSGSFS 18
#define REG_ERR 19
#define REG_TRAPNO 20
#define REG_OLDMASK 21
#define REG_CR2 22
#endif

#define __NGREG 23
#if defined(_DEFAULT_SOURCE)
#define NGREG __NGREG
typedef unsigned long gregset_t[__NGREG];
#endif

struct _fpxreg {
	unsigned short __pollution(significand)[4];
	unsigned short __pollution(exponent);
	unsigned short __padding[3];
};

struct _xmmreg {
	__mlibc_uint32 __pollution(element)[4];
};

struct _fpstate {
	__mlibc_uint16 __pollution(cwd);
	__mlibc_uint16 __pollution(swd);
	__mlibc_uint16 __pollution(ftw);
	__mlibc_uint16 __pollution(fop);
	__mlibc_uint64 __pollution(rip);
	__mlibc_uint64 __pollution(rdp);
	__mlibc_uint32 __pollution(mxcsr);
	__mlibc_uint32 __pollution(mxcr_mask);
	struct _fpxreg _st[8];
	struct _xmmreg _xmm[16];
	__mlibc_uint32 __padding[24];
};

#if defined(_DEFAULT_SOURCE)
struct sigcontext {
	unsigned long r8, r9, r10, r11, r12, r13, r14, r15;
	unsigned long rdi, rsi, rbp, rbx, rdx, rax, rcx, rsp, rip, eflags;
	unsigned short cs, gs, fs, ss;
	unsigned long err, trapno, oldmask, cr2;
	struct _fpstate *fpstate;
	unsigned long __reserved1[8];
};
#endif

typedef struct {
	unsigned long __pollution(gregs)[__NGREG];
	struct _libc_fpstate *__pollution(fpregs);
	unsigned long __reserved1[8];
} mcontext_t;

typedef struct __ucontext {
	unsigned long __pollution(uc_flags);
	struct __ucontext *uc_link;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
	sigset_t uc_sigmask;
	struct _fpstate __fpregs_mem;
	unsigned long __ssp[4];
} ucontext_t;

#elif defined(__aarch64__)

#if defined(_DEFAULT_SOURCE)
#define NGREG 34
typedef unsigned long greg_t, gregset_t[NGREG];

#define FPSIMD_MAGIC 0x46508001
#define ESR_MAGIC 0x45535201
#define EXTRA_MAGIC 0x45585401
#define SVE_MAGIC 0x53564501

struct _aarch64_ctx {
	__mlibc_uint32 magic;
	__mlibc_uint32 size;
};

struct fpsimd_context {
	struct _aarch64_ctx head;
	__mlibc_uint32 fpsr;
	__mlibc_uint32 fpcr;
	__uint128_t vregs[32];
};

struct esr_context {
	struct _aarch64_ctx head;
	__mlibc_uint64 esr;
};

struct extra_context {
	struct _aarch64_ctx head;
	__mlibc_uint64 datap;
	__mlibc_uint32 size;
	__mlibc_uint32 __reserved[3];
};
#endif

typedef struct {
	__mlibc_uint64 __pollution(fault_address);
	__mlibc_uint64 __pollution(regs)[31];
	__mlibc_uint64 __pollution(sp);
	__mlibc_uint64 __pollution(pc);
	__mlibc_uint64 __pollution(pstate);
	__mlibc_uint8 __reserved[4096] __attribute__((__aligned__(16)));
} mcontext_t;

typedef struct __ucontext {
	unsigned long __pollution(uc_flags);
	struct __ucontext *uc_link;
	stack_t uc_stack;
	sigset_t uc_sigmask;
	mcontext_t uc_mcontext;
} ucontext_t;

#elif defined(__riscv) && __riscv_xlen == 64

#if defined(_DEFAULT_SOURCE)
#define NGREG 32
#define REG_PC 0
#define REG_RA 1
#define REG_SP 2
#define REG_TP 4
#define REG_S0 8
#define REG_S1 9
#define REG_A0 10
#define REG_S2 18
#define REG_NARGS 8
typedef unsigned long greg_t, gregset_t[NGREG];
#endif

struct __riscv_mc_f_ext_state {
	__mlibc_uint32 __pollution(f)[32];
	__mlibc_uint32 __pollution(fcsr);
};

struct __riscv_mc_d_ext_state {
	__mlibc_uint64 __pollution(f)[32];
	__mlibc_uint32 __pollution(fcsr);
};

struct __riscv_mc_q_ext_state {
	__mlibc_uint64 __pollution(f)[64] __attribute__((__aligned__(16)));
	__mlibc_uint32 __pollution(fcsr);
	__mlibc_uint32 __reserved[3];
};

union __riscv_mc_fp_state {
	struct __riscv_mc_f_ext_state __f;
	struct __riscv_mc_d_ext_state __d;
	struct __riscv_mc_q_ext_state __q;
};

typedef struct {
	unsigned long __pollution(gregs)[32];
	union __riscv_mc_fp_state __pollution(fpregs);
} mcontext_t;

typedef struct __ucontext {
	unsigned long __pollution(uc_flags);
	struct __ucontext *uc_link;
	stack_t uc_stack;
	sigset_t uc_sigmask;
	mcontext_t uc_mcontext;
} ucontext_t;

#elif defined(__loongarch64)

#if defined(_DEFAULT_SOURCE)
#define NGREG 32
#define REG_RA 1
#define REG_SP 3
#define REG_A0 4
#define REG_S0 23
#define REG_S1 24
#define REG_S2 25
#define REG_NARGS 8
typedef unsigned long greg_t, gregset_t[NGREG];
#endif

typedef struct {
	unsigned long __pollution(pc);
	unsigned long __pollution(gregs)[32];
	unsigned int __pollution(flags);
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
	__extension__ unsigned long __pollution(extcontext)[0] __attribute__((__aligned__(16)));
#endif
} mcontext_t;

typedef struct __ucontext {
	unsigned long __pollution(uc_flags);
	struct __ucontext *uc_link;
	stack_t uc_stack;
	sigset_t uc_sigmask;
	long __uc_pad;
	__extension__ mcontext_t uc_mcontext;
} ucontext_t;

#else
#error "Missing architecture specific code."
#endif

#undef __pollution

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SIGNAL_H */
