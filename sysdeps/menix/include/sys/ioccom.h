#ifndef _SYS_IOCCOM_H
#define _SYS_IOCCOM_H

#define IOCPARM_SHIFT 13
#define IOCPARM_MASK ((1 << IOCPARM_SHIFT) - 1)
#define IOCPARM_LEN(x) (((x) >> 16) & IOCPARM_MASK)
#define IOCBASECMD(x) ((x) & ~(IOCPARM_MASK << 16))
#define IOCGROUP(x) (((x) >> 8) & 0xff)
#define IOCPARM_MAX (1 << IOCPARM_SHIFT)

#define IOC_VOID 0x20000000UL
#define IOC_OUT 0x40000000UL
#define IOC_IN 0x80000000UL
#define IOC_INOUT (IOC_IN | IOC_OUT)
#define IOC_DIRMASK (IOC_VOID | IOC_OUT | IOC_IN)

#define _IOC(inout, group, num, len)                                                               \
	((unsigned long)((inout) | (((len) & IOCPARM_MASK) << 16) | ((group) << 8) | (num)))

#define _IO(g, n) _IOC(IOC_VOID, (g), (n), 0)
#define _IOWINT(g, n) _IOC(IOC_VOID, (g), (n), sizeof(int))
#define _IOR(g, n, t) _IOC(IOC_OUT, (g), (n), sizeof(t))
#define _IOW(g, n, t) _IOC(IOC_IN, (g), (n), sizeof(t))
#define _IOWR(g, n, t) _IOC(IOC_INOUT, (g), (n), sizeof(t))
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
