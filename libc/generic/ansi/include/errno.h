
#ifndef _ERRNO_H
#define _ERRNO_H

#define EDOM 1
#define EILSEQ 2
#define ERANGE 3

extern int errno;

#include <mlibc/posix_errno.h>

#endif // _ERRNO_H

