#ifndef _SYS_PROCFS_H
#define _SYS_PROCFS_H

#include <sys/user.h>
#include <sys/time.h>
#include <abi-bits/pid_t.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long elf_greg_t;

#define ELF_NGREG (sizeof (struct user_regs_struct) / sizeof (elf_greg_t))
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef struct user_fpregs_struct elf_fpregset_t;
typedef elf_gregset_t prgregset_t;
typedef struct user_fpregs_struct prfpregset_t;

#define ELF_PRARGSZ 80

struct elf_siginfo {
	int si_signo;
	int si_code;
	int si_errno;
};

struct elf_prstatus {
	struct elf_siginfo pr_info;
	short int pr_cursig;
	unsigned long int pr_sigpend;
	unsigned long int pr_sighold;
	pid_t pr_pid;
	pid_t pr_ppid;
	pid_t pr_pgrp;
	pid_t pr_sid;
	struct timeval pr_utime;
	struct timeval pr_stime;
	struct timeval pr_cutime;
	struct timeval pr_cstime;
	elf_gregset_t pr_reg;
	int pr_fpvalid;
};

typedef pid_t lwpid_t;
typedef void *psaddr_t;
typedef struct elf_prstatus prstatus_t;

#ifdef __cplusplus
}
#endif

#endif
