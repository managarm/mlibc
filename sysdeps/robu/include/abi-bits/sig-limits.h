#ifndef _ABIBITS_SIG_LIMITS_H
#define _ABIBITS_SIG_LIMITS_H

#include <mlibc-config.h>

#define NSIG_MAX 1024

#if defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN
#define NZERO 20
#endif /* defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN */

#endif /*_ABIBITS_SIG_LIMITS_H */
