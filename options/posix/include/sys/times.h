#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H

// TODO: Only define the clock_t type.
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tms {
	clock_t tms_utime;
	clock_t tms_stime;
	clock_t tms_cutime;
	clock_t tms_cstime;
};

clock_t times(struct tms *);

#ifdef __cplusplus
}
#endif

#endif // _SYS_TIMES_H
