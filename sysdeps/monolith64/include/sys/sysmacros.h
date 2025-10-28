#ifndef _SYS_SYSMACROS_H
#define _SYS_SYSMACROS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * These are the same bit allocations used by Linux and glibc:
 *  - major: bits 31–20 and 19–8 combined
 *  - minor: bits 19–8 and 7–0 combined
 * You can change this if your kernel encodes device numbers differently.
 */

#define major(dev) (((unsigned int)((dev) >> 8)) & 0xFFF)
#define minor(dev) (((unsigned int)((dev) & 0xFF)) | (((dev) >> 12) & 0xFFF00))
#define makedev(maj, min) \
    ((((maj) & 0xFFF) << 8) | ((min) & 0xFF) | (((min) & 0xFFF00) << 12))

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SYSMACROS_H */

