#ifndef _SYS_UIO_H
#define _SYS_UIO_H

#include <bits/posix/ssize_t.h>
#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t readv(int, const struct iovec *, int);
ssize_t writev(int, const struct iovec *, int);

#ifdef __cplusplus
}
#endif

#endif // _SYS_UIO_H
