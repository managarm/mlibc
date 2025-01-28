#ifndef _ERROR_H
#define _ERROR_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C"{
#endif

#ifndef __MLIBC_ABI_ONLY

__attribute__((__format__(__printf__, 3, 4)))
void error(int __status, int __errnum, const char *__format, ...);
__attribute__((__format__(__printf__, 5, 6)))
void error_at_line(int __status, int __errnum, const char *__filename, unsigned int __linenum, const char *__format, ...);

extern unsigned int error_message_count;
extern int error_one_per_line;
extern void (*error_print_progname)(void);
	
#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif
	
#endif /* _ERROR_H */
