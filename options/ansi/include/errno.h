#ifndef _ERRNO_H
#define _ERRNO_H

#include <bits/feature.h>
#include <abi-bits/errno.h>

// Some programs define their own errno as an "extern int" if it is not a macro.
#define errno __mlibc_errno
extern __thread int __mlibc_errno;

// Linux extensions.

extern char *program_invocation_name;
extern char *program_invocation_short_name;
extern char *__progname;
extern char *__progname_full;

#endif // _ERRNO_H
