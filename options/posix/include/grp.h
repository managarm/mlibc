#ifndef _GRP_H
#define _GRP_H

#include <stddef.h>
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

void endgrent(void);
struct group *getgrent(void);
struct group *getgrgid(gid_t);
int getgrgid_r(gid_t, struct group *, char *, size_t, struct group **);
struct group *getgrnam(const char *);
int getgrnam_r(const char *, struct group *, char *, size_t, struct group **);
void setgrent(void);

int setgroups(size_t size, const gid_t *list);
int initgroups(const char *user, gid_t group);

#ifdef __cplusplus
}
#endif

#endif // _GRP_H
