#ifndef _ERROR_H
#define _ERROR_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C"{
#endif

void error(int status, int errnum, const char *format, ...);
void error_at_line(int status, int errnum, const char *filename, unsigned int linenum, const char *format, ...);

extern unsigned int error_message_count;
extern int error_one_per_line;
extern void (*error_print_progname)(void);
	
#ifdef __cplusplus
}
#endif
	
#endif /* _ERROR_H */
