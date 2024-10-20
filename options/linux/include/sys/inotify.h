#ifndef _SYS_INOTIFY_H
#define _SYS_INOTIFY_H

#include <stdint.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/inotify.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IN_ACCESS 0x1
#define IN_ATTRIB 0x4
#define IN_CLOSE_WRITE 0x8
#define IN_CLOSE_NOWRITE 0x10
#define IN_CREATE 0x100
#define IN_DELETE 0x200
#define IN_DELETE_SELF 0x400
#define IN_MODIFY 0x2
#define IN_MOVE_SELF 0x800
#define IN_MOVED_FROM 0x40
#define IN_MOVED_TO 0x80
#define IN_OPEN 0x20
#define IN_MOVE (IN_MOVED_FROM | IN_MOVED_TO)
#define IN_CLOSE (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)
#define IN_DONT_FOLLOW 0x2000000
#define IN_EXCL_UNLINK 0x4000000
#define IN_MASK_ADD 0x20000000
#define IN_ONESHOT 0x80000000
#define IN_ONLYDIR 0x1000000
#define IN_IGNORED 0x8000
#define IN_ISDIR 0x40000000
#define IN_Q_OVERFLOW 0x4000
#define IN_UNMOUNT 0x2000

#define IN_ALL_EVENTS (IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE | \
			 IN_CLOSE_NOWRITE | IN_OPEN | IN_MOVED_FROM | \
			 IN_MOVED_TO | IN_DELETE | IN_CREATE | IN_DELETE_SELF | \
			 IN_MOVE_SELF)

struct inotify_event {
	int wd;
	unsigned int mask;
	unsigned int cookie;
	unsigned int len;

/*
 * glibc uses a flexible array member here, but we get a warning and they don't:
 * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=117241
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	char name[];
#pragma GCC diagnostic pop
};

#ifndef __MLIBC_ABI_ONLY

int inotify_init(void);
int inotify_init1(int __flags);
int inotify_add_watch(int __ifd, const char *__path, uint32_t __mask);
int inotify_rm_watch(int __ifd, int __wd);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /*_SYS_INOTIFY_H */

