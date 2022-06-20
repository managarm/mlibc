#ifndef _SYS_POLL_H
#define _SYS_POLL_H

#include <bits/types.h>
#include <abi-bits/poll.h>

typedef __mlibc_size nfds_t;

#ifdef __cplusplus
extern "C" {
#endif

struct pollfd {
	int fd;
	short events;
	short revents;
};

int poll(struct pollfd *, nfds_t, int);

#ifdef __cplusplus
}
#endif

#endif // _SYS_POLL_H
