#ifndef  _UTMP_H
#define  _UTMP_H

#include <abi-bits/pid_t.h>
#include <bits/posix/timeval.h>
#include <bits/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EMPTY 0
#define RUN_LVL 1
#define BOOT_TIME 2
#define NEW_TIME 3
#define OLD_TIME 4
#define INIT_PROCESS 5
#define LOGIN_PROCESS 6
#define USER_PROCESS 7
#define DEAD_PROCESS 8
#define ACCOUNTING 9

#define UT_LINESIZE 32
#define UT_NAMESIZE 32
#define UT_HOSTSIZE 256

struct exit_status {
	short int e_termination;
	short int e_exit;
};

struct utmp {
	short ut_type;
	pid_t ut_pid;
	char ut_line[UT_LINESIZE];
	char ut_id[4];
	char ut_user[UT_NAMESIZE];
	char ut_host[UT_HOSTSIZE];
	struct exit_status ut_exit;
	long   ut_session;
	struct timeval ut_tv;
	__mlibc_int32 ut_addr_v6[4];
	char __unused[20];
};

/* Hacks for compability reasons */
#define ut_name ut_user
#ifndef _NO_UT_TIME
#define ut_time ut_tv.tv_sec
#endif
#define ut_xtime ut_tv.tv_sec
#define ut_addr ut_addr_v6[0]

void setutent(void);
struct utmp *getutent(void);
void endutent(void);

#ifdef __cplusplus
}
#endif

#endif // _UTMP_H
