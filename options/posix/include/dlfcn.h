
#ifndef _DLFCN_H
#define _DLFCN_H

#define RTLD_LOCAL 0
#define RTLD_NOW 1
#define RTLD_GLOBAL 2
#define RTLD_NOLOAD 4
#define RTLD_NODELETE 8
#define RTLD_DEEPBIND 16
#define RTLD_LAZY 32

#define RTLD_NEXT ((void *)-1)
#define RTLD_DEFAULT ((void *)0)

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

//gnu extension
typedef struct {
	const char *dli_fname;
	void *dli_fbase;
	const char *dli_sname;
	void *dli_saddr;
} Dl_info;

#ifndef __MLIBC_ABI_ONLY

int dladdr(const void *, Dl_info *);
int dlinfo(void *, int, void *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _DLFCN_H

