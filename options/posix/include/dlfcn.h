
#ifndef _DLFCN_H
#define _DLFCN_H

#include <mlibc-config.h>

#define RTLD_LOCAL 0
#define RTLD_LAZY 1
#define RTLD_NOW 2
#define RTLD_NOLOAD 4
#define RTLD_DEEPBIND 8
#define RTLD_GLOBAL 256
#define RTLD_NODELETE 4096

#define RTLD_NEXT ((void *)-1)
#define RTLD_DEFAULT ((void *)0)

#define RTLD_DL_SYMENT 1
#define RTLD_DL_LINKMAP 2

#define RTLD_DI_LINKMAP 2

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int dlclose(void *);
char *dlerror(void);
void *dlopen(const char *, int);
void *dlsym(void *__restrict, const char *__restrict);
void *dlvsym(void *__restrict, const char *__restrict, const char *__restrict);

#endif /* !__MLIBC_ABI_ONLY */

#if defined(_GNU_SOURCE) && __MLIBC_GLIBC_OPTION

//gnu extension
typedef struct {
	const char *dli_fname;
	void *dli_fbase;
	const char *dli_sname;
	void *dli_saddr;
} Dl_info;

#ifndef __MLIBC_ABI_ONLY

int dladdr(const void *__ptr, Dl_info *__out);
int dladdr1(const void *__ptr, Dl_info *__out, void **__extra, int __flags);
int dlinfo(void *__restrict __handle, int __request, void *__restrict __info);

#endif /* !__MLIBC_ABI_ONLY */

#endif /* defined(_GNU_SOURCE) && __MLIBC_GLIBC_OPTION */

#ifdef __cplusplus
}
#endif

#endif // _DLFCN_H

