
#ifndef _SYS_RANDOM_H
#define _SYS_RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

#define GRND_RANDOM 1
#define GRND_NONBLOCK 2

#include <bits/ssize_t.h>
#include <bits/size_t.h>

#ifndef __MLIBC_ABI_ONLY

ssize_t getrandom(void *__buffer, size_t __max_size, unsigned int __flags);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /*_SYS_RANDOM_H */

