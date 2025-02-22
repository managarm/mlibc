#ifndef _BITS_LINUX_UNISTD_H
#define _BITS_LINUX_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int dup3(int __fd, int __newfd, int __flags);
int vhangup(void);
int getdtablesize(void);
int syncfs(int __fd);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _BITS_LINUX_UNISTD_H */
