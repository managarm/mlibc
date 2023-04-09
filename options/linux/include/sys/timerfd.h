#ifndef _SYS_TIMERFD_H
#define _SYS_TIMERFD_H

// musl includes those. Refactor and remove them?
#include <time.h>
#include <fcntl.h>

#define TFD_NONBLOCK O_NONBLOCK
#define TFD_CLOEXEC O_CLOEXEC

#define TFD_TIMER_ABSTIME 1
#define TFD_TIMER_CANCEL_ON_SET (1 << 1)

#ifdef __cplusplus
extern "C" {
#endif

struct itimerspec;

#ifndef __MLIBC_ABI_ONLY

int timerfd_create(int, int);
int timerfd_settime(int, int, const struct itimerspec *, struct itimerspec *);
int timerfd_gettime(int, struct itimerspec *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SYS_TIMERFD_H
