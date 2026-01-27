#ifndef _BITS_LINUX_UNISTD_H
#define _BITS_LINUX_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc-config.h>
#include <bits/off_t.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>

#ifndef __MLIBC_ABI_ONLY

#if defined(_DEFAULT_SOURCE)
int vhangup(void);
#endif

#if defined(_GNU_SOURCE)
int syncfs(int __fd);
ssize_t copy_file_range(int __fd_in, off_t *__off_in, int __fd_out, off_t *__off_out, size_t __size, unsigned int __flags);
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _BITS_LINUX_UNISTD_H */
