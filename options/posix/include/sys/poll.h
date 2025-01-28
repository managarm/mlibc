#ifndef _SYS_POLL_H
#define _SYS_POLL_H

#include <bits/types.h>
#include <bits/sigset_t.h>
#include <bits/ansi/timespec.h>
#include <abi-bits/poll.h>
#include <abi-bits/signal.h>
#include <mlibc-config.h>

typedef __mlibc_size nfds_t;

#ifdef __cplusplus
extern "C" {
#endif

struct pollfd {
	int fd;
	short events;
	short revents;
};

#ifndef __MLIBC_ABI_ONLY

int poll(struct pollfd *__fds, nfds_t __nfds, int __timeout);

#if __MLIBC_LINUX_OPTION
int ppoll(struct pollfd *__fds, nfds_t __nfds,
		const struct timespec *__timeout_ts, const sigset_t *__sigmask);
#endif /* __MLIBC_LINUX_OPTION */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_POLL_H */
