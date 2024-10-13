
#ifndef _SYS_SENDFILE_H_
#define _SYS_SENDFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

#ifndef __MLIBC_ABI_ONLY

ssize_t sendfile(int __out_fd, int __in_fd, off_t *__offset, size_t __size);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SENDFILE_H_ */

