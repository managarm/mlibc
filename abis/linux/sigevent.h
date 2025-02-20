#ifndef _ABIBITS_SIGEVENT_H
#define _ABIBITS_SIGEVENT_H

#include <abi-bits/sigval.h>
#include <abi-bits/pid_t.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sigevent {
	union sigval sigev_value;
	int sigev_notify;
	int sigev_signo;
	void (*sigev_notify_function)(union sigval);
	struct __mlibc_threadattr *sigev_notify_attributes;
	pid_t sigev_notify_thread_id;
};

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SIGEVENT_H */
