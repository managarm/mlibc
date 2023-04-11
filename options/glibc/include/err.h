#ifndef  _ERR_H
#define  _ERR_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

void warn(const char *, ...);
void vwarn(const char *, va_list);
void warnx(const char *, ...);
void vwarnx(const char *, va_list);

__attribute__((__noreturn__)) void err(int, const char *, ...);
__attribute__((__noreturn__)) void verr(int, const char *, va_list);
__attribute__((__noreturn__)) void errx(int, const char *, ...);
__attribute__((__noreturn__)) void verrx(int, const char *, va_list);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _ERR_H

