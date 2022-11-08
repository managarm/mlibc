#ifndef _LINUX_UNISTD_H
#define _LINUX_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

int dup3(int fd, int newfd, int flags);
int vhangup(void);
int getdtablesize(void);
int syncfs(int fd);

#ifdef __cplusplus
}
#endif

#endif // _LINUX_UNISTD_H
