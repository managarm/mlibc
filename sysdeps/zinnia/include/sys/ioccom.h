#ifndef _SYS_IOCCOM_H
#define _SYS_IOCCOM_H

#include <zinnia/ioctls.h>

#define IOCPARM_SHIFT 14
#define IOCPARM_MASK ((1 << IOCPARM_SHIFT) - 1)
#define IOCPARM_LEN(x) __IOC_LEN(x)
#define IOCBASECMD(x) ((x) & ~(IOCPARM_MASK << 16))
#define IOCGROUP(x) __IOC_GROUP(x)
#define IOCPARM_MAX (1 << IOCPARM_SHIFT)

#define IOC_VOID ((unsigned long)__IOC_VOID)
#define IOC_OUT ((unsigned long)__IOC_OUT)
#define IOC_IN ((unsigned long)__IOC_IN)
#define IOC_INOUT (IOC_IN | IOC_OUT)
#define IOC_DIRMASK (IOC_VOID | IOC_OUT | IOC_IN)

#define _IOC(inout, group, num, len) ((unsigned long)__IOC((inout), (group), (num), (len)))
#define _IO(g, n) ((unsigned long)__IO((g), (n)))
#define _IOWINT(g, n) ((unsigned long)__IOWINT((g), (n)))
#define _IOR(g, n, t) ((unsigned long)__IOR((g), (n), t))
#define _IOW(g, n, t) ((unsigned long)__IOW((g), (n), t))
#define _IOWR(g, n, t) ((unsigned long)__IOWR((g), (n), t))
#define _IOC_NEWLEN(ioc, len) (((~(IOCPARM_MASK << 16)) & (ioc)) | (((len) & IOCPARM_MASK) << 16))
#define _IOC_NEWTYPE(ioc, type) _IOC_NEWLEN((ioc), sizeof(type))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SYS_IOCTL_H
int ioctl(int __fd, unsigned long __request, ...);
#endif /* _SYS_IOCTL_H */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_IOCCOM_H */
