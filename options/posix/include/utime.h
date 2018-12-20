#ifndef _UTIME_H
#define _UTIME_H

#include <bits/ansi/time_t.h>

#ifdef __cplusplus
extern "C" {
#endif

struct utimbuf {
	time_t actime;
	time_t modtime;
};

int utime(const char *, const struct utimbuf *);

#ifdef __cplusplus
}
#endif

#endif // _UTIME_H
