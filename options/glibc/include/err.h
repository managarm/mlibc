#ifndef  _ERR_H
#define  _ERR_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

__attribute__((__format__(__printf__, 1, 2))) void warn(const char *__format, ...);
__attribute__((__format__(__printf__, 1, 0))) void vwarn(const char *__format, va_list __args);
__attribute__((__format__(__printf__, 1, 2))) void warnx(const char *__format, ...);
__attribute__((__format__(__printf__, 1, 0))) void vwarnx(const char *__format, va_list __args);

__attribute__((__noreturn__, __format__(__printf__, 2, 3)))
void err(int __errnum, const char *__format, ...);
__attribute__((__noreturn__, __format__(__printf__, 2, 0)))
void verr(int __errnum, const char *__format, va_list __args);
__attribute__((__noreturn__, , __format__(__printf__, 2, 3)))
void errx(int __errnum, const char *__format, ...);
__attribute__((__noreturn__, __format__(__printf__, 2, 0)))
void verrx(int __errnum, const char *__format, va_list __args);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _ERR_H */

