#ifndef _ABIBITS_WAIT_H
#define _ABIBITS_WAIT_H

#if __MLIBC_BUILDING_MLIBC
#warning wait.h is a broken header originally from the deprecated "mlibc" ABI. \
    We suggest to use abis/linux/wait.h instead. \
    Note that this will potentially require kernel changes.
#endif

#define WCONTINUED 1
#define WNOHANG 2
#define WUNTRACED 4
#define WEXITED 8
#define WNOWAIT 16
#define WSTOPPED 32

#define __WALL 0x40000000
#define __WCLONE 0x80000000

#define WCOREFLAG 0x80

#define WEXITSTATUS(x) ((x) & 0x000000FF)
#define WIFCONTINUED(x) ((x) & 0x00000100)
#define WIFEXITED(x) ((x) & 0x00000200)
#define WIFSIGNALED(x) ((x) & 0x00000400)
#define WIFSTOPPED(x) ((x) & 0x00000800)
#define WSTOPSIG(x) (((x) & 0x00FF0000) >> 16)
#define WTERMSIG(x) (((x) & 0xFF000000) >> 24)
#define WCOREDUMP(x) ((x) & WCOREFLAG)

#endif /*_ABIBITS_WAIT_H */
