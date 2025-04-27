#ifndef _ABIBITS_SIGNAL_H
#define _ABIBITS_SIGNAL_H

#include <stdint.h>
#include <time.h>
#include <abi-bits/sigevent.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/uid_t.h>
#include <bits/size_t.h>

#define POLL_IN 1
#define POLL_OUT 2
#define POLL_MSG 3
#define POLL_ERR 4
#define POLL_PRI 5
#define POLL_HUP 6

/* struct taken from musl. */

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

/* Required for sys_sigaction sysdep. */
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

/* Argument for signal() */
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

typedef struct {
	unsigned long sig[1024 / (8 * sizeof(long))];
} sigset_t;

/* constants for sigprocmask() */
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

/* constants for sigev_notify of struct sigevent */
#define SIGEV_SIGNAL 0
#define SIGEV_NONE 1
#define SIGEV_THREAD 2
#define SIGEV_THREAD_ID 4

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

#if defined(__i386__)
#define REG_GS 0
#define REG_FS 1
#define REG_ES 2
#define REG_DS 3
#define REG_EDI 4
#define REG_ESI 5
#define REG_EBP 6
#define REG_ESP 7
#define REG_EBX 8
#define REG_EDX 9
#define REG_ECX 10
#define REG_EAX 11
#define REG_TRAPNO 12
#define REG_ERR 13
#define REG_EIP 14
#define REG_CS 15
#define REG_EFL 16
#define REG_UESP 17
#define REG_SS 18
#define NGREG 19
#elif defined(__x86_64__)
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
#endif

#include <bits/threads.h>

struct sigaction {
	union {
		void (*sa_handler)(int);
		void (*sa_sigaction)(int, siginfo_t *, void *);
	} __sa_handler;
	unsigned long sa_flags;
	void (*sa_restorer)(void);
	sigset_t sa_mask;
};

#define sa_handler __sa_handler.sa_handler
#define sa_sigaction __sa_handler.sa_sigaction

/* Taken from the linux kernel headers */

#if defined(__x86_64__) || defined(__i386__)

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
#if defined(__x86_64__)
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
#elif defined(__i386__)
	uint32_t cw;
	uint32_t sw;
	uint32_t tag;
	uint32_t ipoff;
	uint32_t cssel;
	uint32_t dataoff;
	uint32_t datasel;
	struct _fpreg _st[8];
	uint16_t status;
	uint16_t magic;

	/* FXSR FPU */

	uint32_t _fxsr_env[6];
	uint32_t mxscr;
	uint32_t reserved;
	struct _fpxreg _fxsr_st[8];
	struct _xmmreg _xmm[8];

	uint32_t padding2[56];
#endif
};

struct sigcontext {
#if defined(__x86_64__)
	unsigned long r8, r9, r10, r11, r12, r13, r14, r15;
	unsigned long rdi, rsi, rbp, rbx, rdx, rax, rcx, rsp, rip, eflags;
	unsigned short cs, gs, fs, __pad0;
	unsigned long err, trapno, oldmask, cr2;
	struct _fpstate *fpstate;
	unsigned long __reserved1[8];
#elif defined(__i386__)
	unsigned short gs, __gsh, fs, __fsh, es, __esh, ds, __dsh;
	unsigned long edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned long trapno, err, eip;
	unsigned short cs, __csh;
	unsigned long eflags, esp_at_signal;
	unsigned short ss, __ssh;
	struct _fpstate *fpstate;
	unsigned long oldmask, cr2;
#endif
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
/* Definitions from Linux kernel headers. */

#define NGREG 32

enum {
  REG_PC = 0,
#define REG_PC REG_PC
  REG_RA = 1,
#define REG_RA REG_RA
  REG_SP = 2,
#define REG_SP REG_SP
  REG_TP = 4,
#define REG_TP REG_TP
  REG_S0 = 8,
#define REG_S0 REG_S0
  REG_A0 = 10
#define REG_A0 REG_A0
};

struct __riscv_f_ext_state {
	uint32_t f[32];
	uint32_t fcsr;
};

struct __riscv_d_ext_state {
	uint64_t f[32];
	uint32_t fcsr;
};

struct __riscv_q_ext_state {
	uint64_t f[64] __attribute__((__aligned__(16)));
	uint32_t fcsr;
	uint32_t reserved[3];
};

union __riscv_fp_state {
	struct __riscv_f_ext_state f;
	struct __riscv_d_ext_state d;
	struct __riscv_q_ext_state q;
};

typedef unsigned long __riscv_mc_gp_state[NGREG];

typedef struct sigcontext {
	__riscv_mc_gp_state gregs;
	union __riscv_fp_state fpregs;
} mcontext_t;

typedef struct __ucontext {
	unsigned long uc_flags;
	struct ucontext	*uc_link;
	stack_t uc_stack;
	sigset_t uc_sigmask;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	uint8_t __unused[1024 / 8 - sizeof(sigset_t)];
#pragma GCC diagnostic pop
	mcontext_t uc_mcontext;
} ucontext_t;

#elif defined (__aarch64__)

#define NGREG 34

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

#elif defined (__m68k__)

/* taken from musl */

#if defined(_GNU_SOURCE) || defined(__MLIBC_BUILDING_MLIBC)
enum { R_D0 = 0 };
#define R_D0 R_D0
enum { R_D1 = 1 };
#define R_D1 R_D1
enum { R_D2 = 2 };
#define R_D2 R_D2
enum { R_D3 = 3 };
#define R_D3 R_D3
enum { R_D4 = 4 };
#define R_D4 R_D4
enum { R_D5 = 5 };
#define R_D5 R_D5
enum { R_D6 = 6 };
#define R_D6 R_D6
enum { R_D7 = 7 };
#define R_D7 R_D7
enum { R_A0 = 8 };
#define R_A0 R_A0
enum { R_A1 = 9 };
#define R_A1 R_A1
enum { R_A2 = 10 };
#define R_A2 R_A2
enum { R_A3 = 11 };
#define R_A3 R_A3
enum { R_A4 = 12 };
#define R_A4 R_A4
enum { R_A5 = 13 };
#define R_A5 R_A5
enum { R_A6 = 14 };
#define R_A6 R_A6
enum { R_A7 = 15 };
#define R_A7 R_A7
enum { R_SP = 15 };
#define R_SP R_SP
enum { R_PC = 16 };
#define R_PC R_PC
enum { R_PS = 17 };
#define R_PS R_PS
#endif

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || defined(__MLIBC_BUILDING_MLIBC)

struct sigcontext {
	unsigned long sc_mask, sc_usp, sc_d0, sc_d1, sc_a0, sc_a1;
	unsigned short sc_sr;
	unsigned long sc_pc;
	unsigned short sc_formatvec;
	unsigned long sc_fpregs[6], sc_fpcntl[3];
	unsigned char sc_fpstate[216];
};

typedef int greg_t, gregset_t[18];
typedef struct {
	int f_pcr, f_psr, f_fpiaddr, f_fpregs[8][3];
} fpregset_t;

typedef struct {
	int version;
	gregset_t gregs;
	fpregset_t fpregs;
} mcontext_t;
#else
typedef struct {
	int __version;
	int __gregs[18];
	int __fpregs[27];
} mcontext_t;
#endif

struct sigaltstack {
	void *ss_sp;
	int ss_flags;
	size_t ss_size;
};

typedef struct __ucontext {
	unsigned long uc_flags;
	struct __ucontext *uc_link;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
	long __reserved[80];
	sigset_t uc_sigmask;
} ucontext_t;

#elif defined(__loongarch64)
/* Taken from musl. */

#define NGREG 32
#define REG_RA 1
#define REG_SP 3
#define REG_S0 23
#define REG_S1 24
#define REG_A0 4
#define REG_S2 25
#define REG_NARGS 8

typedef unsigned long greg_t, gregset_t[32];

struct sigcontext {
	unsigned long sc_pc;
	unsigned long sc_regs[32];
	unsigned sc_flags;
	unsigned long sc_extcontext[1] __attribute__((__aligned__(16)));
};

typedef struct {
	unsigned long pc;
	unsigned long gregs[32];
	unsigned flags;
	unsigned long extcontext[1] __attribute__((__aligned__(16)));
} mcontext_t;

struct sigaltstack {
	void *ss_sp;
	int ss_flags;
	size_t ss_size;
};

typedef struct __ucontext {
	unsigned long uc_flags;
	struct __ucontext *uc_link;
	stack_t uc_stack;
	sigset_t uc_sigmask;
	long __uc_pad;
	mcontext_t uc_mcontext;
} ucontext_t;

#else
#error "Missing architecture specific code."
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SIGNAL_H */
