#ifndef _LINUX_KDEV_T_H
#define _LINUX_KDEV_T_H

#define MAJOR(dev)      ((dev) >> 8)
#define MINOR(dev)      ((dev) & 0xff)
#define MKDEV(ma,mi)    ((ma) << 8 | (mi))
#endif /* _LINUX_KDEV_T_H */
