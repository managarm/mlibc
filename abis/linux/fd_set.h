#ifndef _ABIBITS_FD_SET_H
#define _ABIBITS_FD_SET_H

#include <bits/types.h>

#define FD_SETSIZE 1024

typedef long int __fd_mask;
#define __NFDBITS (8 * (int) sizeof (__fd_mask))

typedef struct {
	/* libtirpc reaches into fds_bits directly, so we must keep the name and type exactly as expected.
	 * This now matches the definition in sys/select.h from glibc. */
	__fd_mask fds_bits[FD_SETSIZE / __NFDBITS];
} fd_set;

#endif /* _ABIBITS_FD_SET_H */
