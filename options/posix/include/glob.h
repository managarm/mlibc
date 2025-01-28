
#ifndef _GLOB_H
#define _GLOB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/size_t.h>

#define GLOB_APPEND 0x01
#define GLOB_DOOFFS 0x02
#define GLOB_ERR 0x04
#define GLOB_MARK 0x08
#define GLOB_NOCHECK 0x10
#define GLOB_NOESCAPE 0x20
#define GLOB_NOSORT 0x40
#define GLOB_PERIOD 0x80
#define GLOB_TILDE 0x100
#define GLOB_TILDE_CHECK 0x200
#define GLOB_BRACE 0x400
#define GLOB_NOMAGIC 0x800
#define GLOB_ALTDIRFUNC 0x1000
#define GLOB_ONLYDIR 0x2000
#define GLOB_MAGCHAR 0x4000

#define GLOB_ABORTED 1
#define GLOB_NOMATCH 2
#define GLOB_NOSPACE 3
#define GLOB_NOSYS 4

struct stat;
typedef struct glob_t {
	size_t gl_pathc;
	char **gl_pathv;
	size_t gl_offs;
	int gl_flags;
	void (*gl_closedir) (void *);
	struct dirent *(*gl_readdir) (void *);
	void *(*gl_opendir) (const char *);
	int (*gl_lstat) (const char *__restrict, struct stat *__restrict);
	int (*gl_stat) (const char *__restrict, struct stat *__restrict);
} glob_t;

#ifndef __MLIBC_ABI_ONLY

int glob(const char *__restrict __pattern, int __flags,
		int(*__errfunc)(const char *__epath, int __errnum), struct glob_t *__restrict __pglob);
void globfree(struct glob_t *__pglog);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _GLOB_H */


