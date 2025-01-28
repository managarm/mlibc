#ifndef _SYS_EVENTFD_H
#define _SYS_EVENTFD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <fcntl.h>

typedef uint64_t eventfd_t;

#define EFD_SEMAPHORE 1
#define EFD_CLOEXEC O_CLOEXEC
#define EFD_NONBLOCK O_NONBLOCK

#ifndef __MLIBC_ABI_ONLY

int eventfd(unsigned int __initval, int __flags);
int eventfd_read(int __fd, eventfd_t *__value);
int eventfd_write(int __fd, eventfd_t __value);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_EVENTFD_H */
