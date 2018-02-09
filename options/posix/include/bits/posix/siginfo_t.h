
#ifndef MLIBC_SIGINFO_T_H
#define MLIBC_SIGINFO_T_H

#include <bits/posix/pid_t.h>
#include <bits/posix/uid_t.h>

union sigval {
	int sival_int;
	void *sival_ptr;
};

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

#endif // MLIBC_SIGINFO_T_H
