#ifndef _ABIBITS_SIGEVENT_H
#define _ABIBITS_SIGEVENT_H

#include <abi-bits/sigval.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sigevent {
	int sigev_notify;
	int sigev_signo;
	union sigval sigev_value;
	void (*sigev_notify_function)(union sigval);
	/* MISSING: sigev_notify_attributes */
};

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SIGEVENT_H */
