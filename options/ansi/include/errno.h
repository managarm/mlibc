#ifndef _ERRNO_H
#define _ERRNO_H

#include <abi-bits/errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

/* Some programs define their own errno as an "extern int" if it is not a macro. */
#define errno __mlibc_errno
extern __thread int __mlibc_errno;

int *__errno_location(void);

/* Linux extensions. */

extern char *program_invocation_name;
extern char *program_invocation_short_name;
extern char *__progname;
extern char *__progname_full;

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _ERRNO_H */
