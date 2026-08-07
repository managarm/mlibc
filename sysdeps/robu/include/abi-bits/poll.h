#ifndef _ABIBITS_POLL_H
#define _ABIBITS_POLL_H

#define POLLIN 0x0001
#define POLLPRI 0x0002
#define POLLOUT 0x0004
#define POLLERR 0x0008
#define POLLHUP 0x0010
#define POLLNVAL 0x0020
#define POLLRDNORM 0x0040
#define POLLRDBAND 0x0080
#define POLLRDHUP 0x2000

#if defined(__x86_64__) || defined(__i386__) || defined(__aarch64__) || defined(__riscv) || defined(__loongarch64)
#define POLLWRNORM 0x0100
#define POLLWRBAND 0x0200
#elif defined(__m68k__)
#define POLLWRNORM POLLOUT
#define POLLWRBAND 0x0100
#else
#warning "Missing poll.h definitions for this architecture!"
#endif

#endif /* _ABIBITS_POLL_H */
