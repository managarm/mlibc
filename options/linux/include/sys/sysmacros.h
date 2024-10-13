#ifndef _SYS_SYSMACROS_H
#define _SYS_SYSMACROS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/inline-definition.h>

__MLIBC_INLINE_DEFINITION unsigned int __mlibc_dev_major(
		unsigned long long int __dev) {
  return ((__dev >> 8) & 0xfff) | ((unsigned int)(__dev >> 32) & ~0xfff);
}

__MLIBC_INLINE_DEFINITION unsigned int __mlibc_dev_minor(
		unsigned long long int __dev) {
  return (__dev & 0xff) | ((unsigned int)(__dev >> 12) & ~0xff);
}

__MLIBC_INLINE_DEFINITION unsigned long long int __mlibc_dev_makedev(
		unsigned int __major, unsigned int __minor) {
  return ((__minor & 0xff) | ((__major & 0xfff) << 8)
	  | (((unsigned long long int)(__minor & ~0xff)) << 12)
	  | (((unsigned long long int)(__major & ~0xfff)) << 32));
}

#define major(dev) __mlibc_dev_major(dev)
#define minor(dev) __mlibc_dev_minor(dev)
#define makedev(major, minor) __mlibc_dev_makedev(major, minor)

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SYSMACROS_H */
