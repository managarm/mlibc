
#ifndef  _UTMPX_H
#define  _UTMPX_H

#include <abi-bits/pid_t.h>
#include <bits/posix/timeval.h>

// Struct definition taken from musl
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

void updwtmpx(const char *, const struct utmpx *);

#endif // _UTMPX_H
