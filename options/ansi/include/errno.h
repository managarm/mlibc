#ifndef _ERRNO_H
#define _ERRNO_H

#include <bits/feature.h>

#define EDOM 1
#define EILSEQ 2
#define ERANGE 3

// Some programs define their own errno as an "extern int" if it is not a macro.
#define errno __mlibc_errno
extern __thread int __mlibc_errno;

// Linux extensions.

extern char *program_invocation_name;
extern char *program_invocation_short_name;

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_errno.h>
#endif

#endif // _ERRNO_H
