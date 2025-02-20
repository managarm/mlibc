
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

int dlclose(void *__handle);
char *dlerror(void);
void *dlopen(const char *__name, int __flags);
void *dlsym(void *__restrict __handle, const char *__restrict __name);
void *dlvsym(void *__restrict __handle, const char *__restrict __name, const char *__restrict __version);

#endif /* !__MLIBC_ABI_ONLY */

#if defined(_GNU_SOURCE) && __MLIBC_GLIBC_OPTION

/*gnu extension */
typedef struct {
	const char *dli_fname;
	void *dli_fbase;
	const char *dli_sname;
	void *dli_saddr;
} Dl_info;

#if defined(__i386__)
#define DLFO_STRUCT_HAS_EH_DBASE 1
#define DLFO_STRUCT_HAS_EH_COUNT 0
#define DLFO_EH_SEGMENT_TYPE PT_GNU_EH_FRAME
#elif defined(__arm__)
#define DLFO_STRUCT_HAS_EH_DBASE 0
#define DLFO_STRUCT_HAS_EH_COUNT 1
#define DLFO_EH_SEGMENT_TYPE PT_ARM_EXIDX
#else
#define DLFO_STRUCT_HAS_EH_DBASE 0
#define DLFO_STRUCT_HAS_EH_COUNT 0
#define DLFO_EH_SEGMENT_TYPE PT_GNU_EH_FRAME
#endif

struct dl_find_object {
	unsigned long long dlfo_flags;
	void *dlfo_map_start;
	void *dlfo_map_end;
	struct link_map *dlfo_link_map;
	void *dlfo_eh_frame;
#if DLFO_STRUCT_HAS_EH_DBASE
	void *dlfo_eh_dbase;
#if __INTPTR_WIDTH__ == 32
	unsigned int __unused0;
#endif
#endif
#if DLFO_STRUCT_HAS_EH_COUNT
	int dlfo_eh_count;
	unsigned int __unused1;
#endif
	unsigned long long __dlfo_unused[7];
};

#ifndef __MLIBC_ABI_ONLY

int dladdr(const void *__ptr, Dl_info *__out);
int dladdr1(const void *__ptr, Dl_info *__out, void **__extra, int __flags);
int dlinfo(void *__restrict __handle, int __request, void *__restrict __info);
int _dl_find_object(void *__address, struct dl_find_object *__result);

#endif /* !__MLIBC_ABI_ONLY */

#endif /* defined(_GNU_SOURCE) && __MLIBC_GLIBC_OPTION */

#ifdef __cplusplus
}
#endif

#endif /* _DLFCN_H */

