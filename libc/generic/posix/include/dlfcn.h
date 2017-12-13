
#ifndef _DLFCN_H
#define _DLFCN_H

#define RTDL_LAZY 0
#define RTDL_NOW 1
#define RTDL_GLOBAL 2
#define RTDL_LOCAL 0

#ifdef __cplusplus
extern "C" {
#endif

int dlclose(void *);
char *dlerror(void);
void *dlopen(const char *, int);
void *dlsym(void *__restrict, const char *__restrict);

//gnu extension
typedef struct {
	const char *dli_fname;
	void *dli_fbase;
	const char *dli_sname;
	void *dli_saddr;
} Dl_info;

int dladdr(const void *, Dl_info *);
int dlinfo(void *, int, void *);

#ifdef __cplusplus
}
#endif

#endif // _DLFCN_H

