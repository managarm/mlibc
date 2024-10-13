#ifndef  _PRINTF_H
#define  _PRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdarg.h>

#ifndef __MLIBC_ABI_ONLY

/* This seems to be a glibc thing, so constants are from glibc */
size_t parse_printf_format(const char * __restrict __format, size_t __size, int * __restrict __argtypes);

#endif /* !__MLIBC_ABI_ONLY */

enum {
	PA_INT,
	PA_CHAR,
	PA_WCHAR,
	PA_STRING,
	PA_WSTRING,
	PA_POINTER,
	PA_FLOAT,
	PA_DOUBLE,
	PA_LAST
};

#define PA_FLAG_MASK			0xff00
#define PA_FLAG_LONG_LONG		(1 << 8)
#define PA_FLAG_LONG_DOUBLE		PA_FLAG_LONG_LONG
#define PA_FLAG_LONG			(1 << 9)
#define PA_FLAG_SHORT			(1 << 10)
#define PA_FLAG_PTR				(1 << 11)

#ifdef __cplusplus
}
#endif

#endif
