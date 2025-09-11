#ifndef _SYS_SYSMACROS_H
#define _SYS_SYSMACROS_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned int __mlibc_dev_major(unsigned long long int __dev);

unsigned int __mlibc_dev_minor(unsigned long long int __dev);

unsigned long long int __mlibc_dev_makedev(unsigned int __major, unsigned int __minor);

#define major(dev) __mlibc_dev_major(dev)
#define minor(dev) __mlibc_dev_minor(dev)
#define makedev(major, minor) __mlibc_dev_makedev(major, minor)

#ifdef __cplusplus
}
#endif

#endif // _SYS_SYSMACROS_H
