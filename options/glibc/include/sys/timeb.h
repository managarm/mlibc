#ifndef _SYS_TIMEB_H
#define _SYS_TIMEB_H

#include <bits/ansi/time_t.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timeb {
	time_t time;
	unsigned short int millitm;
	short int timezone;
	short int dstflag;
};

#ifdef __cplusplus
}
#endif

#endif // _SYS_TIMEB_H
