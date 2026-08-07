#ifndef MLIBC_ABIS_LINUX_XATTR_H
#define MLIBC_ABIS_LINUX_XATTR_H

#include <mlibc-config.h>

#if !__MLIBC_LINUX_OPTION
#  error "<sys/xattr.h> is inherently Linux specific. Enable the Linux option or do not use this header."
#endif /* !__MLIBC_LINUX_OPTION */

/* __USE_KERNEL_XATTR_DEFS is exported when XATTR_* are emitted, and
 * __UAPI_DEF_XATTR is used to determine the behaviour of the
 * <linux/xattr.h> header (through <linux/libc-compat.h>), if it's set
 * to 1, the header exports xattr defines and __USE_KERNEL_XATTR_DEFS.
 * This applies for pretty much all other defines in libc-compat.h
 * AFAICT.
 */
#ifndef __USE_KERNEL_XATTR_DEFS
enum {
	XATTR_CREATE = 1,
#define XATTR_CREATE XATTR_CREATE
	XATTR_REPLACE = 2
#define XATTR_REPLACE XATTR_REPLACE
};
#	define __UAPI_DEF_XATTR 0
#endif

#endif /* MLIBC_ABIS_LINUX_XATTR_H */
