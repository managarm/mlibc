
#ifndef _ERRNO_H
#define _ERRNO_H

#define EDOM 1
#define EILSEQ 2
#define ERANGE 3

// if we don't define errno as a macro some programs assume it's an "extern int"
#define errno __mlibc_errno
extern __thread int __mlibc_errno;

#include <mlibc/posix_errno.h>

#endif // _ERRNO_H

