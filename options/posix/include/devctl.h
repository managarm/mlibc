#ifndef _DEVCTL_H
#define _DEVCTL_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int posix_devctl(int __fd, int __dcmd, void *__restrict __dev_data_ptr, size_t __nbyte, int *__restrict __dev_info_ptr);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _DEVCTL_H */
