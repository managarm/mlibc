#ifndef _SYS_KD_H
#define _SYS_KD_H

/* Make sure the <linux/types.h> header is not loaded.  */
#ifndef _LINUX_TYPES_H
# define _LINUX_TYPES_H         1
# define __undef_LINUX_TYPES_H
#endif

#include <linux/kd.h>

#ifdef __undef_LINUX_TYPES_H
# undef _LINUX_TYPES_H
# undef __undef_LINUX_TYPES_H
#endif

#endif /* _SYS_KD_H */
