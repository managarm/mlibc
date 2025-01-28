
#ifndef _DIRENT_H
#define _DIRENT_H

#include <abi-bits/ino_t.h>
#include <bits/off_t.h>
#include <bits/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WHT 14

#define __MLIBC_DIRENT_BODY ino_t d_ino; \
			off_t d_off; \
			unsigned short d_reclen; \
			unsigned char d_type; \
			char d_name[1024];

struct dirent {
	__MLIBC_DIRENT_BODY
};

struct dirent64 {
	__MLIBC_DIRENT_BODY
};

#define d_fileno d_ino

#undef __MLIBC_DIRENT_BODY

#define IFTODT(mode) (((mode) & 0170000) >> 12)

struct __mlibc_dir_struct {
	int __handle;
	__mlibc_size __ent_next;
	__mlibc_size __ent_limit;
	char __ent_buffer[2048];
	struct dirent __current;
};

typedef struct __mlibc_dir_struct DIR;

#ifndef __MLIBC_ABI_ONLY

int alphasort(const struct dirent **__a, const struct dirent **__b);
int closedir(DIR *__dirp);
int dirfd(DIR *__dirp);
DIR *fdopendir(int __fd);
DIR *opendir(const char *__pathname);
struct dirent *readdir(DIR *__dirp);
struct dirent64 *readdir64(DIR *__dirp);
int readdir_r(DIR *__restrict __dirp, struct dirent *__restrict __entry, struct dirent **__restrict __res);
void rewinddir(DIR *__dirp);
int scandir(const char *__pathname, struct dirent ***__res, int (*__select)(const struct dirent *__entry),
		int (*__compare)(const struct dirent **__a, const struct dirent **__b));
void seekdir(DIR *__dirp, long __loc);
long telldir(DIR *__dirp);
int versionsort(const struct dirent **__a, const struct dirent **__b);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _DIRENT_H */

