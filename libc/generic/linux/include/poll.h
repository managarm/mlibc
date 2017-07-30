#ifndef _POLL_H
#define _POLL_H

#define POLLIN 0x01
#define POLLOUT 0x02
#define POLLPRI 0x04
#define POLLHUP 0x08
#define POLLERR 0x10
#define POLLRDHUP 0x20
#define POLLNVAL 0x40

#include <mlibc/internal/types.h>

typedef __mlibc_size nfds_t;

#ifdef __cplusplus
extern "C" {
#endif

struct pollfd {
	int fd;
	short events;
	short revents;
};

int poll(struct pollfd *, nfds_t);

#ifdef __cplusplus
}
#endif

#endif // _POLL_H
