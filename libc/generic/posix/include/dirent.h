
#ifndef _DIRENT_H
#define _DIRENT_H

#include <mlibc/ino_t.h>

#ifdef __cplusplus
extern "C" {
#endif

struct __mlibc_Dir { };
typedef struct __mlibc_Dir DIR;

struct dirent {
	ino_t d_ino;
	char d_name[1024];
};

int alphasort(const struct dirent **, const struct dirent **);
int closedir(DIR *);
int dirfd(DIR *);
DIR *fdopendir(int);
DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *__restrict, struct dirent *__restrict, struct dirent **__restrict);
void rewinddir(DIR *);
int scandir(const char *, struct dirent ***, int (*)(const struct dirent *),
		int (*)(const struct dirent **, const struct dirent **));
void seekdir(DIR *, long);
long telldir(DIR *);

#ifdef __cplusplus
}
#endif

#endif // _DIRENT_H

