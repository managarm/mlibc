#ifndef _MLIBC_INTERNAL_SYSMACROS_H
#define _MLIBC_INTERNAL_SYSMACROS_H

#include <bits/inline-definition.h>

#ifdef __cplusplus
extern "C" {
#endif

__MLIBC_INLINE_DEFINITION unsigned int __mlibc_dev_major(unsigned long long int __dev) {
	return ((__dev >> 8) & 0xfff) | ((unsigned int)(__dev >> 32) & ~0xfff);
}

__MLIBC_INLINE_DEFINITION unsigned int __mlibc_dev_minor(unsigned long long int __dev) {
	return (__dev & 0xff) | ((unsigned int)(__dev >> 12) & ~0xff);
}

__MLIBC_INLINE_DEFINITION unsigned long long int
__mlibc_dev_makedev(unsigned int __major, unsigned int __minor) {
	return (
	    (__minor & 0xff) | ((__major & 0xfff) << 8)
	    | (((unsigned long long int)(__minor & ~0xff)) << 12)
	    | (((unsigned long long int)(__major & ~0xfff)) << 32)
	);
}

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_INTERNAL_SYSMACROS_H */
