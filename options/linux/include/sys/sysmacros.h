#ifndef _SYS_SYSMACROS_H
#define _SYS_SYSMACROS_H

#include <bits/sysmacros.h>

#ifdef __cplusplus
extern "C" {
#endif

#define major(dev) __mlibc_dev_major(dev)
#define minor(dev) __mlibc_dev_minor(dev)
#define makedev(major, minor) __mlibc_dev_makedev(major, minor)

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SYSMACROS_H */
