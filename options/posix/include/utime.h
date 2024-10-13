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

#ifndef __MLIBC_ABI_ONLY

int utime(const char *__filename, const struct utimbuf *__times);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _UTIME_H */
