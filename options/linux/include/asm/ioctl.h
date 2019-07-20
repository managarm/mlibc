#ifndef _ASM_IOCTL_H
#define _ASM_IOCTL_H

// ----------------------------------------------------------------------------
// Linux-like macros to generate ioctls.
// This section of the file is adapted from the Linux kernel.
// ----------------------------------------------------------------------------

#define _IOC_NRBITS 8
#define _IOC_TYPEBITS 8
#define _IOC_SIZEBITS 14
#define _IOC_DIRBITS 2

#define _IOC_NRMASK ((1 << _IOC_NRBITS)-1)
#define _IOC_TYPEMASK ((1 << _IOC_TYPEBITS)-1)
#define _IOC_SIZEMASK ((1 << _IOC_SIZEBITS)-1)
#define _IOC_DIRMASK ((1 << _IOC_DIRBITS)-1)

#define _IOC_NRSHIFT 0
#define _IOC_TYPESHIFT (_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT (_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT (_IOC_SIZESHIFT+_IOC_SIZEBITS)

#define _IOC_NONE 0U
#define _IOC_WRITE 1U
#define _IOC_READ 2U

#define _IOC(dir, type, num, size) (((dir) << _IOC_DIRSHIFT) | ((type) << _IOC_TYPESHIFT) \
		| ((num) << _IOC_NRSHIFT) | ((size) << _IOC_SIZESHIFT))

// Linux enfores that the t parameter of _IO{R,W,WR} is actually a type.
// It does not perform this check for _IO{R,W_WR}_BAD.
#define _IO(type, num) _IOC(_IOC_NONE, (type), (num), 0)
#define _IOR(type, num, t) _IOC(_IOC_READ, (type), (num), sizeof(t))
#define _IOW(type, num, t) _IOC(_IOC_WRITE, (type), (num), sizeof(t))
#define _IOWR(type, num, t) _IOC(_IOC_READ | _IOC_WRITE, (type), (num), sizeof(t))
#define _IOR_BAD(type, num, t) _IOC(_IOC_READ, (type), (num), sizeof(t))
#define _IOW_BAD(type, num, t) _IOC(_IOC_WRITE, (type), (num), sizeof(t))
#define _IOWR_BAD(type, num, t) _IOC(_IOC_READ | _IOC_WRITE, (type), (num), sizeof(t))

#define _IOC_DIR(req) (((req) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#define _IOC_TYPE(req) (((req) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#define _IOC_NR(req) (((req) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#define _IOC_SIZE(req) (((req) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)

#define IOC_IN (_IOC_WRITE << _IOC_DIRSHIFT)
#define IOC_OUT (_IOC_READ << _IOC_DIRSHIFT)
#define IOC_INOUT ((_IOC_WRITE | _IOC_READ) << _IOC_DIRSHIFT)
#define IOCSIZE_MASK (_IOC_SIZEMASK << _IOC_SIZESHIFT)
#define IOCSIZE_SHIFT (_IOC_SIZESHIFT)

#endif // _ASM_IOCTL_H
