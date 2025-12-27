#ifndef _MONETARY_H
#define _MONETARY_H

#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/posix/locale_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

ssize_t strfmon(char *__restrict __s, size_t __maxsize, const char *__restrict __format, ...);
ssize_t strfmon_l(char *__restrict __s, size_t __maxsize, locale_t __locale, const char *__restrict __format, ...);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MONETARY_H */

