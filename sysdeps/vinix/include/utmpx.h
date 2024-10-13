
#ifndef  _UTMPX_H
#define  _UTMPX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/pid_t.h>
#include <bits/posix/timeval.h>

#define UTMPX_FILE "/var/run/utmp"
#define WTMPX_FILE "/var/run/wtmp"

/* Struct definition taken from musl */
struct utmpx {
	short ut_type;
	short __ut_pad1;
	pid_t ut_pid;
	char ut_line[32];
	char ut_id[4];
	char ut_user[32];
	char ut_host[256];
	struct {
		short __e_termination;
		short __e_exit;
	} ut_exit;
	int ut_session, __ut_pad2;
	struct timeval ut_tv;
	unsigned ut_addr_v6[4];
	char __unused[20];
};

#ifndef __MLIBC_ABI_ONLY

void updwtmpx(const char *, const struct utmpx *);
int utmpxname(const char *);
struct utmpx *pututxline(const struct utmpx *);
struct utmpx *getutxent(void);
void setutxent(void);
void endutxent(void);

#endif /* !__MLIBC_ABI_ONLY */

#define EMPTY           0
#define RUN_LVL         1
#define BOOT_TIME       2
#define NEW_TIME        3
#define OLD_TIME        4
#define INIT_PROCESS    5
#define LOGIN_PROCESS   6
#define USER_PROCESS    7
#define DEAD_PROCESS    8

#define __UT_HOSTSIZE 256
#define __UT_NAMESIZE 32
#define __UT_LINESIZE 32

#ifdef __cplusplus
}
#endif

#endif /* _UTMPX_H */
