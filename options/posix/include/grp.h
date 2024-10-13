#ifndef _GRP_H
#define _GRP_H

#include <stddef.h>
#include <stdio.h>
#include <abi-bits/gid_t.h>

#ifdef __cplusplus
extern "C" {
#endif

struct group {
	char *gr_name;
	char *gr_passwd;
	gid_t gr_gid;
	char **gr_mem;
};

#ifndef __MLIBC_ABI_ONLY

void endgrent(void);
struct group *getgrent(void);
struct group *getgrgid(gid_t __gid);
int getgrgid_r(gid_t __gid, struct group *__grp, char *__buf, size_t __buflen, struct group **__res);
struct group *getgrnam(const char *__name);
int getgrnam_r(const char *__name, struct group *__grp, char *__buf, size_t __buflen, struct group **__res);
void setgrent(void);
int putgrent(const struct group *__grp, FILE *__stream);
struct group *fgetgrent(FILE *__stream);

int setgroups(size_t __size, const gid_t *__list);
int initgroups(const char *__user, gid_t __group);

/* Non standard extension */
int getgrouplist(const char *__user, gid_t __group, gid_t *__groups, int *__ngroups);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _GRP_H */
