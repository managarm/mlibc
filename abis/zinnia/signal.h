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

#if defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN
#define TRAP_BRKPT 1
#define TRAP_TRACE 2
#endif

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

typedef struct __mlibc_stack {
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

#define __MC_TAG_END 0

struct __mlibc_mcontext_tag {
	__mlibc_uint32 __pollution(magic);
	__mlibc_uint32 __pollution(size);
	__mlibc_uint64 __pollution(offset);
};

#if defined(__x86_64__)

#define __MC_TAG_X86_64_XSAVE 0x58534156
#define __MC_TAG_X86_64_XSAVE_ALIGN 64

struct __mlibc_mcontext_fxsave_state {
	__mlibc_uint16 __pollution(cwd);
	__mlibc_uint16 __pollution(swd);
	__mlibc_uint8 __pollution(ftw);
	__mlibc_uint8 __reserved0;
	__mlibc_uint16 __pollution(fop);
	__mlibc_uint64 __pollution(rip);
	__mlibc_uint64 __pollution(rdp);
	__mlibc_uint32 __pollution(mxcsr);
	__mlibc_uint32 __pollution(mxcsr_mask);
	__mlibc_uint8 __pollution(st)[8][16];
	__mlibc_uint8 __pollution(xmm)[16][16];
	__mlibc_uint8 __reserved1[96];
} __attribute__((__aligned__(16)));

typedef struct __mlibc_mcontext {
	__mlibc_uint64 __pollution(mc_r15);
	__mlibc_uint64 __pollution(mc_r14);
	__mlibc_uint64 __pollution(mc_r13);
	__mlibc_uint64 __pollution(mc_r12);
	__mlibc_uint64 __pollution(mc_r11);
	__mlibc_uint64 __pollution(mc_r10);
	__mlibc_uint64 __pollution(mc_r9);
	__mlibc_uint64 __pollution(mc_r8);
	__mlibc_uint64 __pollution(mc_rsi);
	__mlibc_uint64 __pollution(mc_rdi);
	__mlibc_uint64 __pollution(mc_rbp);
	__mlibc_uint64 __pollution(mc_rdx);
	__mlibc_uint64 __pollution(mc_rcx);
	__mlibc_uint64 __pollution(mc_rbx);
	__mlibc_uint64 __pollution(mc_rax);
	__mlibc_uint64 __pollution(mc_rip);
	__mlibc_uint64 __pollution(mc_rflags);
	__mlibc_uint64 __pollution(mc_rsp);
	__mlibc_uint64 __pollution(mc_fsbase);
	__mlibc_uint64 __pollution(mc_gsbase);
	__mlibc_uint64 __pollution(mc_trapno);
	__mlibc_uint64 __pollution(mc_err);
	__mlibc_uint64 __pollution(mc_cr2);
	__mlibc_uint64 __pad;
	struct __mlibc_mcontext_fxsave_state __pollution(mc_fpregs);
} mcontext_t;

#elif defined(__aarch64__)

#define __MC_TAG_AARCH64_SVE 0x53564501

struct __mlibc_mcontext_fpsimd_state {
	__mlibc_uint32 __pollution(fpsr);
	__mlibc_uint32 __pollution(fpcr);
	__mlibc_uint64 __pad;
	__uint128_t __pollution(vregs)[32];
};

struct __mlibc_mcontext_sve_state {
	__mlibc_uint16 __pollution(vl);
	__mlibc_uint16 __pollution(flags);
	__mlibc_uint16 __reserved[6];
};

typedef struct __mlibc_mcontext {
	__mlibc_uint64 __pollution(mc_x)[31];
	__mlibc_uint64 __pollution(mc_sp);
	__mlibc_uint64 __pollution(mc_pc);
	__mlibc_uint64 __pollution(mc_pstate);
	__mlibc_uint64 __pollution(mc_tpidr);
	__mlibc_uint64 __pollution(mc_esr);
	__mlibc_uint64 __pollution(mc_far);
	__mlibc_uint64 __pad;
	struct __mlibc_mcontext_fpsimd_state __pollution(mc_fpregs);
} mcontext_t;

#elif defined(__riscv) && __riscv_xlen == 64

#define __MC_TAG_RISCV64_V 0x52564656

struct __mlibc_mcontext_f_state {
	__mlibc_uint32 __pollution(f)[32];
	__mlibc_uint32 __pollution(fcsr);
};

struct __mlibc_mcontext_d_state {
	__mlibc_uint64 __pollution(f)[32];
	__mlibc_uint32 __pollution(fcsr);
	__mlibc_uint32 __reserved[3];
};

struct __mlibc_mcontext_q_state {
	__mlibc_uint64 __pollution(f)[64] __attribute__((__aligned__(16)));
	__mlibc_uint32 __pollution(fcsr);
	__mlibc_uint32 __reserved[3];
};

union __mlibc_mcontext_fp_state {
	struct __mlibc_mcontext_f_state __pollution(f);
	struct __mlibc_mcontext_d_state __pollution(d);
	struct __mlibc_mcontext_q_state __pollution(q);
};

typedef struct __mlibc_mcontext {
	__mlibc_uint64 __pollution(mc_x)[32];
	__mlibc_uint64 __pollution(mc_scause);
	__mlibc_uint64 __pollution(mc_stval);
	union __mlibc_mcontext_fp_state __pollution(mc_fpregs);
} mcontext_t;

#else
#error "Missing architecture specific code."
#endif

#undef __pollution

#if defined(_DEFAULT_SOURCE)
#define UC_SIGMASK (1 << 0)
#define UC_STACK (1 << 1)
#define UC_CPU (1 << 2)
#define UC_TLSBASE (1 << 3)
#define UC_TAGS (1 << 4)
#endif

typedef struct __mlibc_ucontext {
	__mlibc_uint32 uc_flags;
	__mlibc_uint32 __pad;
	struct __mlibc_ucontext *uc_link;
	sigset_t uc_sigmask;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
} ucontext_t;

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SIGNAL_H */
