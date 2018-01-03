
#ifndef _SYS_SENDFILE_H_
#define _SYS_SENDFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

ssize_t sendfile(int, int, off_t *, size_t);

#ifdef __cplusplus
}
#endif

#endif // _SYS_SENDFILE_H_

