#ifndef _ABIBITS_ERRNO_H
#define _ABIBITS_ERRNO_H

#include <mlibc-config.h>

#if __MLIBC_LINUX_OPTION
#include <asm/errno.h>
#endif

/* This is mlibc-specific. */
#define EIEIO           4095

#endif /* _ABIBITS_ERRNO_H */
