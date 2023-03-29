#ifndef _ABIBITS_SIGNAL_H
#define _ABIBITS_SIGNAL_H

#include <stdint.h>
#include <time.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/uid_t.h>
#include <bits/size_t.h>

union sigval {
	int sival_int;
	void *sival_ptr;
};

// struct taken from musl.

typedef struct {
	int si_signo, si_errno, si_code;
	union {
		char __pad[128 - 2*sizeof(int) - sizeof(long)];
		struct {
			union {
				struct {
					pid_t si_pid;
					uid_t si_uid;
				} __piduid;
				struct {
					int si_timerid;
					int si_overrun;
				} __timer;
			} __first;
			union {
				union sigval si_value;
				struct {
					int si_status;
					clock_t si_utime, si_stime;
				} __sigchld;
			} __second;
		} __si_common;
		struct {
			void *si_addr;
			short si_addr_lsb;
			union {
				struct {
					void *si_lower;
					void *si_upper;
				} __addr_bnd;
				unsigned si_pkey;
			} __first;
		} __sigfault;
		struct {
			long si_band;
			int si_fd;
		} __sigpoll;
		struct {
			void *si_call_addr;
			int si_syscall;
			unsigned si_arch;
		} __sigsys;
	} __si_fields;
} siginfo_t;
#define si_pid     __si_fields.__si_common.__first.__piduid.si_pid
#define si_uid     __si_fields.__si_common.__first.__piduid.si_uid
#define si_status  __si_fields.__si_common.__second.__sigchld.si_status
#define si_utime   __si_fields.__si_common.__second.__sigchld.si_utime
#define si_stime   __si_fields.__si_common.__second.__sigchld.si_stime
#define si_value   __si_fields.__si_common.__second.si_value
#define si_addr    __si_fields.__sigfault.si_addr
#define si_addr_lsb __si_fields.__sigfault.si_addr_lsb
#define si_lower   __si_fields.__sigfault.__first.__addr_bnd.si_lower
#define si_upper   __si_fields.__sigfault.__first.__addr_bnd.si_upper
#define si_pkey    __si_fields.__sigfault.__first.si_pkey
#define si_band    __si_fields.__sigpoll.si_band
#define si_fd      __si_fields.__sigpoll.si_fd
#define si_timerid __si_fields.__si_common.__first.__timer.si_timerid
#define si_overrun __si_fields.__si_common.__first.__timer.si_overrun
#define si_ptr     si_value.sival_ptr
#define si_int     si_value.sival_int
#define si_call_addr __si_fields.__sigsys.si_call_addr
#define si_syscall __si_fields.__sigsys.si_syscall
#define si_arch    __si_fields.__sigsys.si_arch

// Required for sys_sigaction sysdep.
#define SA_NOCLDSTOP 1
#define SA_NOCLDWAIT 2
#define SA_SIGINFO 4
#define SA_ONSTACK 0x08000000
#define SA_RESTART 0x10000000
#define SA_NODEFER 0x40000000
#define SA_RESETHAND 0x80000000
#define SA_RESTORER 0x04000000

#ifdef __cplusplus
extern "C" {
#endif

// Argument for signal()
typedef void (*__sighandler) (int);

#define SIG_ERR ((__sighandler)(void *)(-1))
#define SIG_DFL ((__sighandler)(void *)(0))
#define SIG_IGN ((__sighandler)(void *)(1))

#define SIGABRT 6
#define SIGFPE 8
#define SIGILL 4
#define SIGINT 2
#define SIGSEGV 11
#define SIGTERM 15
#define SIGPROF 27
#define SIGIO 29
#define SIGPWR 30
#define SIGRTMIN 35
#define SIGRTMAX 64

// TODO: replace this by uint64_t
typedef long sigset_t;

// constants for sigprocmask()
#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#define SIGHUP    1
#define SIGQUIT   3
#define SIGTRAP   5
#define SIGIOT    SIGABRT
#define SIGBUS    7
#define SIGKILL   9
#define SIGUSR1   10
#define SIGUSR2   12
#define SIGPIPE   13
#define SIGALRM   14
#define SIGSTKFLT 16
#define SIGCHLD   17
#define SIGCONT   18
#define SIGSTOP   19
#define SIGTSTP   20
#define SIGTTIN   21
#define SIGTTOU   22
#define SIGURG    23
#define SIGXCPU   24
#define SIGXFSZ   25
#define SIGVTALRM 26
#define SIGWINCH  28
#define SIGPOLL   29
#define SIGSYS    31
#define SIGUNUSED SIGSYS
#define SIGCANCEL 32

#define MINSIGSTKSZ 2048
#define SIGSTKSZ 8192
#define SS_ONSTACK 1
#define SS_DISABLE 2

typedef struct __stack {
	void *ss_sp;
	int ss_flags;
	size_t ss_size;
} stack_t;

// constants for sigev_notify of struct sigevent
#define SIGEV_SIGNAL 0
#define SIGEV_NONE 1
#define SIGEV_THREAD 2

#define SEGV_MAPERR 1
#define SEGV_ACCERR 2

#define BUS_ADRALN 1
#define BUS_ADRERR 2
#define BUS_OBJERR 3
#define BUS_MCEERR_AR 4
#define BUS_MCEERR_AO 5

#define ILL_ILLOPC 1
#define ILL_ILLOPN 2
#define ILL_ILLADR 3
#define ILL_ILLTRP 4
#define ILL_PRVOPC 5
#define ILL_PRVREG 6
#define ILL_COPROC 7
#define ILL_BADSTK 8
#define ILL_BADIADDR 9

#define NSIG 65

#define SI_ASYNCNL (-60)
#define SI_TKILL (-6)
#define SI_SIGIO (-5)
#define SI_ASYNCIO (-4)
#define SI_MESGQ (-3)
#define SI_TIMER (-2)
#define SI_QUEUE (-1)
#define SI_USER 0
#define SI_KERNEL 128

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
#define NGREG 23

struct sigevent {
	union sigval sigev_value;
	int sigev_notify;
	int sigev_signo;
	void (*sigev_notify_function)(union sigval);
	// MISSING: sigev_notify_attributes
};

struct sigaction {
	union {
		void (*sa_handler)(int);
		void (*sa_sigaction)(int, siginfo_t *, void *);
	} __sa_handler;
	sigset_t sa_mask;
	int sa_flags;
	void (*sa_restorer)(void);
};
#define sa_handler __sa_handler.sa_handler
#define sa_sigaction __sa_handler.sa_sigaction

// Taken from the linux kernel headers

#if defined(__x86_64__)

struct _fpreg {
	unsigned short significand[4];
	unsigned short exponent;
};

struct _fpxreg {
	unsigned short significand[4];
	unsigned short exponent;
	unsigned short padding[3];
};

struct _xmmreg {
	uint32_t element[4];
};

struct _fpstate {
	uint16_t cwd;
	uint16_t swd;
	uint16_t ftw;
	uint16_t fop;
	uint64_t rip;
	uint64_t rdp;
	uint32_t mxcsr;
	uint32_t mxcr_mask;
	struct _fpxreg _st[8];
	struct _xmmreg _xmm[16];
	uint32_t padding[24];
};

typedef struct {
	unsigned long gregs[NGREG];
	struct _fpstate *fpregs;
	unsigned long __reserved1[8];
} mcontext_t;

typedef struct __ucontext {
	unsigned long uc_flags;
	struct __ucontext *uc_link;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
	sigset_t uc_sigmask;
} ucontext_t;

#elif defined(__riscv) && __riscv_xlen == 64
// Definitions from Linux kernel headers.

struct __riscv_f_ext_state {
	uint32_t f[32];
	uint32_t fcsr;
};

struct __riscv_d_ext_state {
	uint64_t f[32];
	uint32_t fcsr;
};

struct __riscv_q_ext_state {
	uint64_t f[64] __attribute__((aligned(16)));
	uint32_t fcsr;
	uint32_t reserved[3];
};

union __riscv_fp_state {
	struct __riscv_f_ext_state f;
	struct __riscv_d_ext_state d;
	struct __riscv_q_ext_state q;
};

struct __user_regs_struct {
	unsigned long pc;
	unsigned long ra;
	unsigned long sp;
	unsigned long gp;
	unsigned long tp;
	unsigned long t0;
	unsigned long t1;
	unsigned long t2;
	unsigned long s0;
	unsigned long s1;
	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
	unsigned long a4;
	unsigned long a5;
	unsigned long a6;
	unsigned long a7;
	unsigned long s2;
	unsigned long s3;
	unsigned long s4;
	unsigned long s5;
	unsigned long s6;
	unsigned long s7;
	unsigned long s8;
	unsigned long s9;
	unsigned long s10;
	unsigned long s11;
	unsigned long t3;
	unsigned long t4;
	unsigned long t5;
	unsigned long t6;
};

typedef struct __mcontext {
	struct __user_regs_struct sc_regs;
	union __riscv_fp_state sc_fpregs;
} mcontext_t;

typedef struct __ucontext {
	unsigned long uc_flags;
	struct ucontext	*uc_link;
	stack_t uc_stack;
	sigset_t uc_sigmask;
	uint8_t  __unused[1024 / 8 - sizeof(sigset_t)];
	mcontext_t uc_mcontext;
} ucontext_t;

#elif defined (__aarch64__)

typedef struct sigcontext {
	uint64_t fault_address;
	uint64_t regs[31];
	uint64_t sp;
	uint64_t pc;
	uint64_t pstate;
	uint8_t __reserved[4096];
} mcontext_t;

#define FPSIMD_MAGIC 0x46508001
#define ESR_MAGIC 0x45535201
#define EXTRA_MAGIC 0x45585401
#define SVE_MAGIC 0x53564501
struct _aarch64_ctx {
	uint32_t magic;
	uint32_t size;
};
struct fpsimd_context {
	struct _aarch64_ctx head;
	uint32_t fpsr;
	uint32_t fpcr;
	__uint128_t vregs[32];
};
struct esr_context {
	struct _aarch64_ctx head;
	uint64_t esr;
};
struct extra_context {
	struct _aarch64_ctx head;
	uint64_t datap;
	uint32_t size;
	uint32_t __reserved[3];
};
struct sve_context {
	struct _aarch64_ctx head;
	uint16_t vl;
	uint16_t __reserved[3];
};
#define SVE_VQ_BYTES		16
#define SVE_VQ_MIN		1
#define SVE_VQ_MAX		512
#define SVE_VL_MIN		(SVE_VQ_MIN * SVE_VQ_BYTES)
#define SVE_VL_MAX		(SVE_VQ_MAX * SVE_VQ_BYTES)
#define SVE_NUM_ZREGS		32
#define SVE_NUM_PREGS		16
#define sve_vl_valid(vl) \
	((vl) % SVE_VQ_BYTES == 0 && (vl) >= SVE_VL_MIN && (vl) <= SVE_VL_MAX)
#define sve_vq_from_vl(vl)	((vl) / SVE_VQ_BYTES)
#define sve_vl_from_vq(vq)	((vq) * SVE_VQ_BYTES)
#define SVE_SIG_ZREG_SIZE(vq)	((unsigned)(vq) * SVE_VQ_BYTES)
#define SVE_SIG_PREG_SIZE(vq)	((unsigned)(vq) * (SVE_VQ_BYTES / 8))
#define SVE_SIG_FFR_SIZE(vq)	SVE_SIG_PREG_SIZE(vq)
#define SVE_SIG_REGS_OFFSET					\
	((sizeof(struct sve_context) + (SVE_VQ_BYTES - 1))	\
		/ SVE_VQ_BYTES * SVE_VQ_BYTES)
#define SVE_SIG_ZREGS_OFFSET	SVE_SIG_REGS_OFFSET
#define SVE_SIG_ZREG_OFFSET(vq, n) \
	(SVE_SIG_ZREGS_OFFSET + SVE_SIG_ZREG_SIZE(vq) * (n))
#define SVE_SIG_ZREGS_SIZE(vq) \
	(SVE_SIG_ZREG_OFFSET(vq, SVE_NUM_ZREGS) - SVE_SIG_ZREGS_OFFSET)
#define SVE_SIG_PREGS_OFFSET(vq) \
	(SVE_SIG_ZREGS_OFFSET + SVE_SIG_ZREGS_SIZE(vq))
#define SVE_SIG_PREG_OFFSET(vq, n) \
	(SVE_SIG_PREGS_OFFSET(vq) + SVE_SIG_PREG_SIZE(vq) * (n))
#define SVE_SIG_PREGS_SIZE(vq) \
	(SVE_SIG_PREG_OFFSET(vq, SVE_NUM_PREGS) - SVE_SIG_PREGS_OFFSET(vq))
#define SVE_SIG_FFR_OFFSET(vq) \
	(SVE_SIG_PREGS_OFFSET(vq) + SVE_SIG_PREGS_SIZE(vq))
#define SVE_SIG_REGS_SIZE(vq) \
	(SVE_SIG_FFR_OFFSET(vq) + SVE_SIG_FFR_SIZE(vq) - SVE_SIG_REGS_OFFSET)
#define SVE_SIG_CONTEXT_SIZE(vq) (SVE_SIG_REGS_OFFSET + SVE_SIG_REGS_SIZE(vq))

typedef struct __ucontext {
	unsigned long uc_flags;
	struct __ucontext *uc_link;
	stack_t uc_stack;
	sigset_t uc_sigmask;
	mcontext_t uc_mcontext;
} ucontext_t;

#else
#error "Missing architecture specific code."
#endif

#ifdef __cplusplus
}
#endif

#endif // _ABIBITS_SIGNAL_H
