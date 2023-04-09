#ifndef _LINUX_UNISTD_H
#define _LINUX_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int dup3(int fd, int newfd, int flags);
int vhangup(void);
int getdtablesize(void);
int syncfs(int fd);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _LINUX_UNISTD_H
